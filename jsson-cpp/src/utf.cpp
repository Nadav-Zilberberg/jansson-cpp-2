#include "../include/utf8.hpp"
#include <stdexcept>

namespace Utf8 {

bool isValid(const std::string& str)
{
    size_t i = 0;
    while (i < str.size()) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        size_t len = 0;

        if (c < 0x80) {
            len = 1;
        } else if (0xC0 <= c && c <= 0xDF) {
            len = 2;
        } else if (0xE0 <= c && c <= 0xEF) {
            len = 3;
        } else if (0xF0 <= c && c <= 0xF4) {
            len = 4;
        } else {
            return false; // Invalid leading byte
        }

        if (i + len > str.size()) {
            return false; // Truncated sequence
        }

        // Check continuation bytes
        for (size_t j = 1; j < len; ++j) {
            unsigned char cont = static_cast<unsigned char>(str[i + j]);
            if (!(0x80 <= cont && cont <= 0xBF)) {
                return false; // Invalid continuation byte
            }
        }

        // Validate code point range and overlong encoding
        int32_t cp = 0;
        for (size_t j = 0; j < len; ++j) {
            cp = (cp << 6) + (str[i + j] & 0x3F);
        }

        if (len == 2 && cp < 0x80) return false; // Overlong encoding
        if (len == 3 && cp < 0x800) return false;
        if (len == 4 && cp < 0x10000) return false;
        if (cp > 0x10FFFF) return false; // Beyond Unicode range
        if (cp >= 0xD800 && cp <= 0xDFFF) return false; // Surrogate

        i += len;
    }
    return true;
}

size_t encode(int32_t cp, std::string& out)
{
    if (cp < 0 || cp > 0x10FFFF) {
        throw std::out_of_range("Code point out of range");
    }

    if (cp < 0x80) {
        out.push_back(static_cast<char>(cp));
        return 1;
    } else if (cp < 0x800) {
        out.push_back(static_cast<char>(0xC0 + ((cp >> 6) & 0x1F)));
        out.push_back(static_cast<char>(0x80 + (cp & 0x3F)));
        return 2;
    } else if (cp < 0x10000) {
        out.push_back(static_cast<char>(0xE0 + ((cp >> 12) & 0x0F)));
        out.push_back(static_cast<char>(0x80 + ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 + (cp & 0x3F)));
        return 3;
    } else {
        out.push_back(static_cast<char>(0xF0 + ((cp >> 18) & 0x07)));
        out.push_back(static_cast<char>(0x80 + ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 + ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 + (cp & 0x3F)));
        return 4;
    }
}

std::optional<char32_t> decode(const std::string& str, size_t& index)
{
    if (index >= str.size()) {
        return std::nullopt;
    }

    unsigned char c = static_cast<unsigned char>(str[index]);
    size_t len = 0;

    if (c < 0x80) {
        len = 1;
    } else if (0xC0 <= c && c <= 0xDF) {
        len = 2;
    } else if (0xE0 <= c && c <= 0xEF) {
        len = 3;
    } else if (0xF0 <= c && c <= 0xF4) {
        len = 4;
    } else {
        return std::nullopt; // Invalid leading byte
    }

    if (index + len > str.size()) {
        return std::nullopt; // Truncated
    }

    // Check continuation bytes
    for (size_t i = 1; i < len; ++i) {
        unsigned char cont = static_cast<unsigned char>(str[index + i]);
        if (!(0x80 <= cont && cont <= 0xBF)) {
            return std::nullopt; // Invalid continuation
        }
    }

    // Decode code point
    int32_t cp = 0;
    for (size_t i = 0; i < len; ++i) {
        cp = (cp << 6) + (str[index + i] & 0x3F);
    }

    // Validate code point
    if ((len == 2 && cp < 0x80) ||
        (len == 3 && cp < 0x800) ||
        (len == 4 && cp < 0x10000) ||
        cp > 0x10FFFF ||
        (cp >= 0xD800 && cp <= 0xDFFF)) {
        return std::nullopt; // Overlong or invalid
    }

    index += len;
    return static_cast<char32_t>(cp);
}

} // namespace Utf8