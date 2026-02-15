#ifndef JSSON_MEMORY_HPP
#define JSSON_MEMORY_HPP

#include <memory>
#include <cstdlib>
#include <cstddef>
#include <type_traits>

namespace jsson {
namespace memory {

/**
 * @brief Simple memory allocation wrapper using RAII.
 *
 * This class encapsulates a memory allocation request and provides
 * RAII semantics for managing the lifetime of the allocated memory.
 */
class Allocator {
public:
    Allocator() = default;
    ~Allocator() = default;

    // Non-copyable, movable
    Allocator(const Allocator&) = delete;
    Allocator& operator=(const Allocator&) = delete;
    Allocator(Allocator&&) = default;
    Allocator& operator=(Allocator&&) = default;

    /**
     * @brief Allocate an array of type T with the given size.
     * @tparam T The type of element to allocate.
     * @param count Number of elements to allocate.
     * @return std::unique_ptr<T[]> owning the allocated array.
     */
    template <typename T>
    static std::unique_ptr<T[]> make_unique_array(std::size_t count) noexcept {
        return std::make_unique<T[]>(count);
    }

    /**
     * @brief Wrapper around std::malloc that uses RAII.
     * @param size Number of bytes to allocate.
     * @return std::unique_ptr<void, decltype(&std::free)> owning the allocated block.
     */
    static std::unique_ptr<void, decltype(&std::free)> malloc(std::size_t size) noexcept {
        return std::unique_ptr<void, decltype(&std::free)>(std::malloc(size), &std::free);
    }

    /**
     * @brief Wrapper around std::free.
     * @param ptr Pointer to deallocate.
     */
    static void free(std::unique_ptr<void, decltype(&std::free)>& ptr) noexcept {
        std::free(ptr.release());
    }

    /**
     * @brief Wrapper around std::realloc.
     * @param ptr Pointer to memory block to reallocate.
     * @param new_size New size in bytes.
     * @return std::unique_ptr<void, decltype(&std::free)> owning the reallocated block.
     */
    static std::unique_ptr<void, decltype(&std::free)> realloc(std::unique_ptr<void, decltype(&std::free)>& ptr,
                                                              std::size_t new_size) noexcept {
        return std::unique_ptr<void, decltype(&std::free)>(std::realloc(ptr.release(), new_size));
    }

    /**
     * @brief Allocate a block of memory with the given size and alignment.
     * @param alignment Alignment requirement (must be power of two).
     * @param size Size of the allocation in bytes.
     * @return std::unique_ptr<void, decltype(&std::free)> owning the allocated block.
     */
    static std::unique_ptr<void, decltype(&std::free)> aligned_alloc(std::size_t alignment,
                                                                   std::size_t size) noexcept {
        return std::unique_ptr<void, decltype(&std::free)>(std::aligned_alloc(alignment, size));
    }
};

} // namespace memory
} // namespace jsson

#endif // JSSON_MEMORY_HPP