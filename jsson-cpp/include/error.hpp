#ifndef JSSON_ERROR_HPP
#define JSSON_ERROR_HPP

#include <string>
#include <system_error>

namespace jsson {

enum class JsonErrorCode {
    // Map json_error_code values
    Success = 0,
    InvalidArgument,
    InvalidNumber,
    InvalidUTF8,
    OutOfMemory,
    StackOverflow,
    CannotOpenFile,
    InvalidSyntax,
    InvalidFormat,
    WrongType,
    NullCharacter,
    NullValue,
    NullByteInKey,
    DuplicateKey,
    NumericOverflow,
    ItemNotFound,
    IndexOutOfRange,
    PrematureEndOfInput,
    EndOfInputExpected,
    Unknown
};

std::string error_message(JsonErrorCode code);

class JsonError : public std::system_error {
public:
    explicit JsonError(JsonErrorCode code, const std::string& message = "")
        : std::system_error(static_cast<int>(code), std::generic_category(), message) {}
};

} // namespace jsson

#endif // JSSON_ERROR_HPP