#ifndef JASSON_MEMORY_HPP
#define JASSON_MEMORY_HPP

#include <memory>
#include <stdexcept>

namespace jasson {

/**
 * Simple memory pool that wraps C-style allocation functions with
 * modern C++ smart pointers.
 *
 * The pool itself does not pre‑allocate a large buffer; instead it
 * provides type‑safe `allocate` helpers that return `std::unique_ptr`
 * owning the allocated memory.  This replaces direct `malloc`,
 * `calloc`, `realloc` and `free` usage throughout the library.
 */
class MemoryPool {
public:
    MemoryPool() = default;
    ~MemoryPool() = default;

    // No copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    // Move is allowed
    MemoryPool(MemoryPool&&) = default;
    MemoryPool& operator=(MemoryPool&&) = default;

    /**
     * Allocate an array of `count` objects of type `T`.
     *
     * @return `std::unique_ptr<T[]>` that owns the memory.
     * @throws `std::bad_alloc` if allocation fails.
     */
    template <typename T>
    std::unique_ptr<T[]> allocate_array(std::size_t count) {
        if (count == 0) return nullptr;
        return std::make_unique<T[]>(count);
    }

    /**
     * Allocate a single object of type `T`.
     *
     * @return `std::unique_ptr<T>` that owns the object.
     * @throws `std::bad_alloc` if allocation fails.
     */
    template <typename T>
    std::unique_ptr<T> allocate_object() {
        return std::make_unique<T>();
    }

    /**
     * Re‑allocate a previously allocated block.
     *
     * @param ptr Existing smart pointer (may be null).
     * @param new_count New number of elements.
     * @return New `std::unique_ptr<T[]>` owning the re‑allocated memory.
     */
    template <typename T>
    std::unique_ptr<T[]> reallocate_array(std::unique_ptr<T[]> ptr, std::size_t new_count) {
        if (new_count == 0) return nullptr;
        auto new_ptr = std::make_unique<T[]>(new_count);
        if (ptr && new_count <= ptr.get_deleter() ? std::size_t{0} : new_count) {
            // Simple copy of existing elements (if any)
            std::size_t copy_cnt = std::min<std::size_t>(new_count, get_size(ptr.get()));
            // In practice we would need the original size; this stub just returns new_ptr.
        }
        return new_ptr;
    }

private:
    /**
     * Helper to obtain the size of an allocated block.
     * This is a placeholder; real implementation would store size metadata.
     */
    static std::size_t get_size(void*) {
        return 0; // Not used in this simplified example.
    }
};

} // namespace jasson

#endif // JASSON_MEMORY_HPP
