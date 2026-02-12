#include "error.hpp"
#include <system_error>
#include <string>

namespace jsson {

// Implement error_message function
std::string error_message(JsonErrorCode code) {
    switch (code) {
        case JsonErrorCode::Success: return "Success";
        case JsonErrorCode::InvalidArgument: return "Invalid argument";
        case JsonErrorCode::InvalidNumber: return "Invalid number";
        case JsonErrorCode::InvalidUTF8: return "Invalid UTF-8";
        case JsonErrorCode::OutOfMemory: return "Out of memory";
        case JsonErrorCode::StackOverflow: return "Stack overflow";
        case JsonErrorCode::CannotOpenFile: return "Cannot open file";
        case JsonErrorCode::InvalidSyntax: return "Invalid syntax";
        case JsonErrorCode::InvalidFormat: return "Invalid format";
        case JsonErrorCode::WrongType: return "Wrong type";
        case JsonErrorCode::NullCharacter: return "Null character";
        case JsonErrorCode::NullValue: return "Null value";
        case JsonErrorCode::NullByteInKey: return "Null byte in key";
        case JsonErrorCode::DuplicateKey: return "Duplicate key";
        case JsonErrorCode::NumericOverflow: return "Numeric overflow";
        case JsonErrorCode::ItemNotFound: return "Item not found";
        case JsonErrorCode::IndexOutOfRange: return "Index out of range";
        case JsonErrorCode::PrematureEndOfInput: return "Premature end of input";
        case JsonErrorCode::EndOfInputExpected: return "End of input expected";
        case JsonErrorCode::Unknown: return "Unknown error";
    }
}

// Throw a JsonError with the given code and message
inline void raise_error(const std::string& message, JsonErrorCode code) {
    throw JsonError(code, message);
}

// Helper function to create JsonError with a code
inline JsonError make_json_error(JsonErrorCode code, const std::string& message) {
    return JsonError(code, message);
}

} // namespace jsson