#include "pack_unpack.hpp"
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <bit>
#include <cstring>
#include "../../src/jansson.h"

namespace jsson {

using Buffer = std::vector<std::byte>;

namespace detail {

// Helper to append a value to the buffer
template <typename T>
void append_to_buffer(Buffer& buffer, T&& value) {
    constexpr size_t size = sizeof(T);
    const std::byte* src = reinterpret_cast<const std::byte*>(&value);
    buffer.insert(buffer.end(), src, src + size);
}

// Helper to read a value from the buffer at position pos
template <typename T>
T read_from_buffer(const Buffer& buffer, size_t& pos) {
    if (pos + sizeof(T) > buffer.size()) {
        throw std::out_of_range("Buffer too small");
    }
    T result{};
    std::byte* dst = reinterpret_cast<std::byte*>(&result);
    std::memcpy(dst, &buffer[pos], sizeof(T));
    pos += sizeof(T);
    return result;
}

} // namespace detail

// Public API

template<typename T>
Buffer PackUnpack::pack(BinaryType type, T&& value) {
    Buffer buffer;
    buffer.push_back(static_cast<std::byte>(type));
    
    switch (type) {
        case BinaryType::Integer: {
            detail::append_to_buffer(buffer, static_cast<json_int_t>(value));
            break;
        }
        case BinaryType::Real: {
            detail::append_to_buffer(buffer, static_cast<double>(value));
            break;
        }
        case BinaryType::Boolean: {
            detail::append_to_buffer(buffer, static_cast<bool>(value));
            break;
        }
        case BinaryType::String: {
            // For strings we need to store length and then the string data
            using StringType = std::string;
            detail::append_to_buffer(buffer, static_cast<uint32_t>(value.size()));
            buffer.insert(buffer.end(),
                          reinterpret_cast<const std::byte*>(value.data()),
                          reinterpret_cast<const std::byte*>(value.data() + value.size()));
            break;
        }
        case BinaryType::Null: {
            // Null has no additional data
            break;
        }
        case BinaryType::Error: {
            // Error can carry an error code
            detail::append_to_buffer(buffer, static_cast<int>(value));
            break;
        }
        default: {
            throw std::invalid_argument("Unsupported binary type");
        }
    }
    return buffer;
}

template<typename T>
T PackUnpack::unpack(const Buffer& buffer, size_t& pos) {
    if (pos >= buffer.size()) {
        throw std::out_of_range("No more data in buffer");
    }
    
    auto type_byte = buffer[pos];
    ++pos;
    
    BinaryType type = static_cast<BinaryType>(type_byte);
    
    switch (type) {
        case BinaryType::Integer: {
            return detail::read_from_buffer<json_int_t>(buffer, pos);
        }
        case BinaryType::Real: {
            return detail::read_from_buffer<double>(buffer, pos);
        }
        case BinaryType::Boolean: {
            return detail::read_from_buffer<bool>(buffer, pos);
        }
        case BinaryType::String: {
            uint32_t length = detail::read_from_buffer<uint32_t>(buffer, pos);
            if (pos + length > buffer.size()) {
                throw std::out_of_range("String length exceeds buffer size");
            }
            std::string result(reinterpret_cast<const char*>(&buffer[pos]), length);
            pos += length;
            return result;
        }
        case BinaryType::Null: {
            return T{}; // Return default-constructed T (null)
        }
        case BinaryType::Error: {
            return detail::read_from_buffer<int>(buffer, pos);
        }
        default: {
            throw std::invalid_argument("Unsupported binary type");
        }
    }
}

} // namespace jsson