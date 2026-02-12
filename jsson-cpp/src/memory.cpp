#include "memory.hpp"
#include <cstdlib>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <new> // for std::bad_alloc

namespace jsson {
namespace memory {

template <typename T>
std::unique_ptr<T[]> Allocator::make_unique_array(std::size_t count) noexcept {
    if (count == 0) {
        return std::unique_ptr<T[]>(nullptr);
    }
    try {
        return std::unique_ptr<T[]>(new T[count]());
    } catch (...) {
        return std::unique_ptr<T[]>(nullptr);
    }
}

std::unique_ptr<void, decltype(&std::free)> Allocator::aligned_alloc(
    std::size_t alignment, std::size_t size) noexcept {

    if (alignment == 0 || size == 0) {
        return std::unique_ptr<void, decltype(&std::free)>(nullptr, &std::free);
    }

    // Ensure alignment is a power of two
    if ((alignment & (alignment - 1)) != 0) {
        return std::unique_ptr<void, decltype(&std::free)>(nullptr, &std::free);
    }

    void* ptr = std::aligned_alloc(alignment, size);
    return std::unique_ptr<void, decltype(&std::free)>(ptr, &std::free);
}

std::unique_ptr<void, decltype(&std::free)> Allocator::malloc(std::size_t size) noexcept {
    if (size == 0) {
        return std::unique_ptr<void, decltype(&std::free)>(nullptr, &std::free);
    }
    void* ptr = std::malloc(size);
    return std::unique_ptr<void, decltype(&std::free)>(ptr, &std::free);
}

void Allocator::free(
    std::unique_ptr<void, decltype(&std::free)>& ptr) noexcept {
    // The unique_ptr will automatically call free when it goes out of scope.
    // This function is a placeholder to maintain compatibility.
}

std::unique_ptr<void, decltype(&std::free)> Allocator::realloc(
    std::unique_ptr<void, decltype(&std::free)>& ptr, std::size_t new_size) noexcept {

    if (new_size == 0) {
        ptr.reset();
        return std::unique_ptr<void, decltype(&std::free)>(nullptr, &std::free);
    }

    void* old_ptr = ptr.get();
    void* new_ptr = std::realloc(old_ptr, new_size);
    if (!new_ptr) {
        return std::unique_ptr<void, decltype(&std::free)>(nullptr, &std::free);
    }
    ptr.reset(new_ptr);
    return std::move(ptr);
}


} // namespace memory
} // namespace jsson