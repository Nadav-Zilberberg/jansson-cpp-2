#include "parser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <cctype>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

/* Helper to read entire file into a string using std::getline */
static std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line;
        content += '\n'; // Preserve line breaks
    }
    return content;
}

/* Skip whitespace characters */
void Parser::Parser::skipWhitespace(std::string_view& view) {
    view.remove_prefix(std::distance(view.begin(), std::find_if(view.begin(), view.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    })));
}

/* Parse a JSON literal (true, false, null) */
static std::shared_ptr<JsonValue> parseLiteral(std::string_view& view) {
    Parser::skipWhitespace(view);

    if (view.substr(0, 4) == "true") {
        view.remove_prefix(4);
        return std::make_shared<JsonValue>(true);
    } else if (view.substr(0, 5) == "false") {
        view.remove_prefix(5);
        return std::make_shared<JsonValue>(false);
    } else if (view.substr(0, 4) == "null") {
        view.remove_prefix(4);
        return std::make_shared<JsonValue>();
    }

    throw std::runtime_error("Invalid literal in JSON input");
}

/* Parse a JSON number (integer or double) */
static std::shared_ptr<JsonValue> parseNumber(std::string_view& view) {
    Parser::skipWhitespace(view);
    size_t start = view.size();

    // Capture the full numeric literal into a string
    std::string numStr;
    auto consume = [&](char c) {
        numStr.push_back(c);
        view.remove_prefix(1);
    };

    // Optional leading minus
    if (view.front() == '-') consume('-');

    // Digits
    while (!view.empty() && std::isdigit(static_cast<unsigned char>(view.front()))) consume(view.front());

    // Fractional part
    if (!view.empty() && view.front() == '.') {
        consume('.');
        while (!view.empty() && std::isdigit(static_cast<unsigned char>(view.front()))) consume(view.front());
    }

    // Exponent part
    if (!view.empty() && (view.front() == 'E' || view.front() == 'e')) {
        consume(view.front());
        // Optional sign
        if (!view.empty() && (view.front() == '+' || view.front() == '-')) consume(view.front());
        // Digits
        while (!view.empty() && std::isdigit(static_cast<unsigned char>(view.front()))) consume(view.front());
    }

    // Now parse the captured string as a number
    try {
        // Use std::stod which handles both integer and floating point formats
        double value = std::stod(numStr);
        // Determine if it should be stored as int64_t when it fits and no fractional part/exponent
        if (numStr.find('.') == std::string::npos && numStr.find('e') == std::string::npos && numStr.find('E') == std::string::npos) {
            // Check if it fits in int64_t range
            try {
                int64_t intVal = static_cast<int64_t>(value);
                if (std::to_string(intVal) == numStr) {
                    return std::make_shared<JsonValue>(intVal);
                }
            } catch (...) {
                // Ignore overflow, fall back to double
            }
        }
        return std::make_shared<JsonValue>(value);
    } catch (...) {
        throw std::runtime_error("Failed to parse JSON number: " + numStr);
    }
}

/* Parse a JSON string */
static std::shared_ptr<JsonValue> parseString(std::string_view& view) {
    Parser::skipWhitespace(view);
    if (view.empty() || view.front() != '"') {
        throw std::runtime_error("Expected string literal");
    }

    view.remove_prefix(1); // Skip opening quote
    std::string result;
    while (!view.empty() && view.front() != '"') {
        char c = view.front();
        view.remove_prefix(1);
        if (c == '\\') {
            if (view.empty()) {
                throw std::runtime_error("Unexpected end of string");
            }
            char esc = view.front();
            view.remove_prefix(1);
            switch (esc) {
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case 'u': {
                    // Parse Unicode escape
                    if (view.size() < 4) {
                        throw std::runtime_error("Invalid Unicode escape");
                    }
                    std::string hex = std::string(view.substr(0, 4));
                    view.remove_prefix(4);
                    // Convert hex to int
                    int value = 0;
                    for (char ch : hex) {
                        value <<= 4;
                        if (std::isdigit(static_cast<unsigned char>(ch))) {
                            value += ch - '0';
                        } else if (std::isalpha(static_cast<unsigned char>(ch))) {
                            value += std::tolower(ch) - 'a' + 10;
                        }
                    }
                    // Encode as UTF-8
                    if (value <= 0x7F) {
                        result += static_cast<char>(value);
                    } else if (value <= 0x7FF) {
                        result += static_cast<char>(0xC0 | ((value >> 6) & 0x1F));
                        result += static_cast<char>(0x80 | (value & 0x3F));
                    } else if (value <= 0xFFFF) {
                        result += static_cast<char>(0xE0 | ((value >> 12) & 0x0F));
                        result += static_cast<char>(0x80 | ((value >> 6) & 0x3F));
                        result += static_cast<char>(0x80 | (value & 0x3F));
                    } else {
                        result += static_cast<char>(0xF0 | ((value >> 18) & 0x07));
                        result += static_cast<char>(0x80 | ((value >> 12) & 0x3F));
                        result += static_cast<char>(0x80 | ((value >> 6) & 0x3F));
                        result += static_cast<char>(0x80 | (value & 0x3F));
                    }
                    break;
                }
                default:
                    throw std::runtime_error("Invalid escape sequence");
            }
        } else {
            result += c;
        }
    }

    view.remove_prefix(1); // Skip closing quote
    return std::make_shared<JsonValue>(std::move(result));
}

/* Parse a JSON object */
static std::shared_ptr<JsonValue> parseObject(std::string_view& view) {
    Parser::skipWhitespace(view);
    if (view.empty() || view.front() != '{') {
        throw std::runtime_error("Expected object start");
    }
    view.remove_prefix(1); // Skip '{'

    auto obj = std::make_unique<JsonObject>();
    Parser::skipWhitespace(view);

    if (view.front() == '}') {
        view.remove_prefix(1);
        return std::make_shared<JsonValue>(*obj);
    }

    while (true) {
        // Parse key (string)
        auto keyValue = parseString(view);
        if (!keyValue->isString()) {
            throw std::runtime_error("Expected string key");
        }
        std::string key = keyValue->asString();

        Parser::skipWhitespace(view);
        if (view.front() != ':') {
            throw std::runtime_error("Expected ':' after key");
        }
        view.remove_prefix(1); // Skip ':'

        // Parse value
        auto value = Parser::parseValue(view);
        obj->keys()[key] = std::move(value);

        Parser::skipWhitespace(view);
        if (view.front() == '}') {
            view.remove_prefix(1);
            return std::make_shared<JsonValue>(*obj);
        }
        if (view.front() == ',') {
            view.remove_prefix(1);
            Parser::skipWhitespace(view);
            continue;
        }
        throw std::runtime_error("Expected ',' or '}'");
    }
}

/* Parse a JSON array */
static std::shared_ptr<JsonValue> parseArray(std::string_view& view) {
    Parser::skipWhitespace(view);
    if (view.empty() || view.front() != '[') {
        throw std::runtime_error("Expected array start");
    }
    view.remove_prefix(1); // Skip '['

    auto arr = std::make_unique<JsonArray>();
    Parser::skipWhitespace(view);

    if (view.front() == ']') {
        view.remove_prefix(1);
        return std::make_shared<JsonValue>(*arr);
    }

    while (true) {
        auto element = Parser::parseValue(view);
        arr->data().push_back(std::move(element));

        Parser::skipWhitespace(view);
        if (view.front() == ']') {
            view.remove_prefix(1);
            return std::make_shared<JsonValue>(*arr);
        }
        if (view.front() == ',') {
            view.remove_prefix(1);
            Parser::skipWhitespace(view);
            continue;
        }
        throw std::runtime_error("Expected ',' or ']'");
    }
}

/* Parse a JSON value (object, array, literal, string, number) */
std::shared_ptr<JsonValue> Parser::parseValue(std::string_view& view) {
    Parser::skipWhitespace(view);
    if (view.empty()) {
        throw std::runtime_error("Unexpected end of input");
    }

    char c = view.front();
    if (c == '{') {
        return parseObject(view);
    } else if (c == '[') {
        return parseArray(view);
    } else if (c == '"') {
        return parseString(view);
    } else if (c == 't' || c == 'f' || c == 'n') {
        return parseLiteral(view);
    } else if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) {
        return parseNumber(view);
    } else {
        throw std::runtime_error("Unexpected character");
    }
}

/* Public parse method */
std::shared_ptr<JsonValue> Parser::parse(const std::string& filename) {
    // Read file content
    std::string fileContent = readFile(filename);
    std::string_view view(fileContent);

    // Parse the content
    auto root = Parser::parseValue(view);

    // Ensure we consumed the entire input
    Parser::skipWhitespace(view);
    if (!view.empty()) {
        throw std::runtime_error("Extra data after valid JSON value");
    }

    return root;
}