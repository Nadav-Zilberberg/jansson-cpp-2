#include "error.hpp"
#include "jansson.h"
#include <system_error>
#include <string>

namespace jsson {

// Implement error_message function
std::string error_message(JsonErrorCode code) {
    using enum JsonErrorCode;
    switch (code) {
        case Success: return "Success";
        case InvalidArgument: return "Invalid argument";
        case InvalidNumber: return "Invalid number";
        case InvalidUTF8: return "Invalid UTF-8";
        case OutOfMemory: return "Out of memory";
        case StackOverflow: return "Stack overflow";
        case CannotOpenFile: return "Cannot open file";
        case InvalidSyntax: return "Invalid syntax";
        case InvalidFormat: return "Invalid format";
        case WrongType: return "Wrong type";
        case NullCharacter: return "Null character";
        case NullValue: return "Null value";
        case NullByteInKey: return "Null byte in key";
        case DuplicateKey: return "Duplicate key";
        case NumericOverflow: return "Numeric overflow";
        case ItemNotFound: return "Item not found";
        case IndexOutOfRange: return "Index out of range";
        case PrematureEndOfInput: return "Premature end of input";
        case EndOfInputExpected: return "End of input expected";
        case Unknown: return "Unknown error";
        default: return "Unknown error";
    }
}

// Convert json_error_code to JsonErrorCode and throw JsonError
inline void raise_error(const json_error_t* error) {
    using enum JsonErrorCode;
    // Extract the json_error_code from the error text (last character)
    int c_code = static_cast<unsigned char>(error->text[JSON_ERROR_TEXT_LENGTH - 1]);
    JsonErrorCode j_code = static_cast<JsonErrorCode>(c_code);
    throw JsonError(j_code, error->text);
}

// Helper function to create JsonError from json_error_t
inline JsonError make_json_error(const json_error_t* error) {
    using enum JsonErrorCode;
    int c_code = static_cast<unsigned char>(error->text[JSON_ERROR_TEXT_LENGTH - 1]);
    JsonErrorCode j_code = static_cast<JsonErrorCode>(c_code);
    return JsonError(j_code, error->text);
}

} // namespace jsson