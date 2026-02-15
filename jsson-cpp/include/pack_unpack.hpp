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
    static Buffer pack(BinaryType type, T&& value);

    // Unpack a value of type T starting at position pos
    template<typename T>
    static T unpack(const Buffer& buffer, size_t& pos);
};

} // namespace jsson

#endif // PACK_UNPACK_HPP
