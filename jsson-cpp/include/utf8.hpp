#ifndef UTF8_HPP
#define UTF8_HPP

#include <string>
#include <optional>
#include <cstdint>
#include <stdexcept>

namespace Utf8 {

/// Validates that a string contains well-formed UTF-8.
/// @return true if valid, false otherwise.
bool isValid(const std::string& str);

/**
 * Encodes a Unicode code point as UTF-8.
 *
 * @param cp Unicode code point (must be <= 0x10FFFF and >= 0).
 * @param out Reference to a string that will receive the UTF-8 bytes.
 * @return Number of bytes written.
 * @throws std::out_of_range if cp is outside the valid range.
 */
size_t encode(int32_t cp, std::string& out);

/**
 * Attempts to decode a UTF-8 sequence at the given index.
 *
 * @param str The string to decode.
 * @param index On input, starting index; on output, index after the decoded character.
 * @param cp On output, the Unicode code point if decoding succeeded.
 * @return std::nullopt on error, otherwise the decoded code point.
 */
std::optional<char32_t> decode(const std::string& str, size_t& index);

} // namespace Utf8

#endif // UTF8_HPP