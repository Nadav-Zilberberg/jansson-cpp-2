#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <memory>
#include <stdexcept>
#include <string_view>
#include "json_value.hpp"
namespace jsson {
class Parser {
public:
    /**
     * @brief Parses a JSON file and returns the root JsonValue.
     * @param filename Path to the JSON file.
     * @return Shared pointer to the parsed JsonValue.
     * @throws std::runtime_error on file opening or parsing errors.
     */
    static std::shared_ptr<JsonValue> parse(const std::string& filename);

public:
    // Helper functions for parsing
    static void skipWhitespace(std::string_view view);
    static std::shared_ptr<JsonValue> parseValue(std::string_view view);
    static std::shared_ptr<JsonValue> parseObject(std::string_view view);
    static std::shared_ptr<JsonValue> parseArray(std::string_view view);
    static std::shared_ptr<JsonValue> parseString(std::string_view view);
    static std::shared_ptr<JsonValue> parseLiteral(std::string_view view);
    static std::shared_ptr<JsonValue> parseNumber(std::string_view view);
};
}

#endif // PARSER_HPP