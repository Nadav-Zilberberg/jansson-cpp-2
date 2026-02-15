#ifndef PACK_UNPACK_HPP
#define PACK_UNPACK_HPP

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <bit>
#include <string>
#include <type_traits>
#include <memory>
#include "json_value.hpp"

namespace jsson {

enum class BinaryType : char {
    Integer = 'i',
    Real = 'd',
    Boolean = 'b',
    String = 's',
    Null = 'N',
    Error = 'E'
};

class PackUnpack {
public:
    using Buffer = std::vector<std::byte>;

    // Pack a value of type T with a given BinaryType
    template<typename T>
    static Buffer pack(BinaryType type, T&& value) {
        Buffer buffer;
        switch (type) {
            case BinaryType::Integer: {
                int64_t v = static_cast<int64_t>(value);
                buffer.push_back(static_cast<std::byte>(BinaryType::Integer));
                buffer.push_back(static_cast<std::byte>(v >> 56));
                buffer.push_back(static_cast<std::byte>((v >> 48) & 0xFF));
                buffer.push_back(static_cast<std::byte>((v >> 40) & 0xFF));
                buffer.push_back(static_cast<std::byte>((v >> 32) & 0xFF));
                buffer.push_back(static_cast<std::byte>((v >> 24) & 0xFF));
                buffer.push_back(static_cast<std::byte>((v >> 16) & 0xFF));
                buffer.push_back(static_cast<std::byte>((v >> 8) & 0xFF));
                buffer.push_back(static_cast<std::byte>(v & 0xFF));
                break;
            }
            case BinaryType::Real: {
                double v = static_cast<double>(value);
                buffer.push_back(static_cast<std::byte>(BinaryType::Real));
                uint64_t* ptr = reinterpret_cast<uint64_t*>(&v);
                uint64_t bits = *ptr;
                for (int i = 0; i < 8; ++i) {
                    buffer.push_back(static_cast<std::byte>((bits >> (56 - i * 8)) & 0xFF));
                }
                break;
            }
            case BinaryType::Boolean: {
                bool v = static_cast<bool>(value);
                buffer.push_back(static_cast<std::byte>(BinaryType::Boolean));
                buffer.push_back(v ? static_cast<std::byte>(1) : static_cast<std::byte>(0));
                break;
            }
            case BinaryType::String: {
                std::string v = static_cast<std::string>(value);
                buffer.push_back(static_cast<std::byte>(BinaryType::String));
                buffer.push_back(static_cast<std::byte>(v.size() >> 24));
                buffer.push_back(static_cast<std::byte>((v.size() >> 16) & 0xFF));
                buffer.push_back(static_cast<std::byte>((v.size() >> 8) & 0xFF));
                buffer.push_back(static_cast<std::byte>(v.size() & 0xFF));
                buffer.insert(buffer.end(), v.begin(), v.end());
                break;
            }
            case BinaryType::Null: {
                buffer.push_back(static_cast<std::byte>(BinaryType::Null));
                break;
            }
            case BinaryType::Error: {
                buffer.push_back(static_cast<std::byte>(BinaryType::Error));
                break;
            }
        }
        return buffer;
    }

    // Unpack a value of type T starting at position pos
    template<typename T>
    static T unpack(const Buffer& buffer, size_t& pos) {
        if (pos >= buffer.size()) {
            throw std::out_of_range("Buffer overflow");
        }
        BinaryType type = static_cast<BinaryType>(buffer[pos]);
        ++pos;
        switch (type) {
            case BinaryType::Integer: {
                if (pos + 8 > buffer.size()) {
                    throw std::out_of_range("Buffer overflow");
                }
                int64_t result = 0;
                for (int i = 0; i < 8; ++i) {
                    result = (result << 8) | buffer[pos + i];
                }
                pos += 8;
                return result;
            }
            case BinaryType::Real: {
                if (pos + 8 > buffer.size()) {
                    throw std::out_of_range("Buffer overflow");
                }
                uint64_t bits = 0;
                for (int i = 0; i < 8; ++i) {
                    bits |= static_cast<uint64_t>(buffer[pos + i]) << (56 - i * 8);
                }
                pos += 8;
                double* ptr = reinterpret_cast<double*>(&bits);
                return *ptr;
            }
            case BinaryType::Boolean: {
                if (pos >= buffer.size()) {
                    throw std::out_of_range("Buffer overflow");
                }
                bool result = buffer[pos] != 0;
                ++pos;
                return result;
            }
            case BinaryType::String: {
                if (pos + 4 > buffer.size()) {
                    throw std::out_of_range("Buffer overflow");
                }
                uint32_t length = 0;
                for (int i = 0; i < 4; ++i) {
                    length = (length << 8) | buffer[pos + i];
                }
                pos += 4;
                if (pos + length > buffer.size()) {
                    throw std::out_of_range("Buffer overflow");
                }
                std::string result(buffer.begin() + pos, buffer.begin() + pos + length);
                pos += length;
                return result;
            }
            case BinaryType::Null: {
                return T();
            }
            case BinaryType::Error: {
                throw std::runtime_error("JSON error");
            }
            default: {
                throw std::runtime_error("Unknown binary type");
            }
        }
    }
};

} // namespace jsson

#endif // PACK_UNPACK_HPP