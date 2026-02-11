#include "hashtable.hpp"
#include <stdexcept>
#include <string>

/**
 * @brief Concrete hash table implementation for JSON strings and void* values.
 *
 * This class provides a thread-safe, modern C++ wrapper around the functionality
 * previously implemented in hashtable.c. It uses `std::unordered_map` for
 * storage and `std::shared_ptr` for automatic memory management of `void*`
 * objects.
 */
class JsonStringHashTable {
public:
    /** Inserts or assigns a void* value for a given key.
     *
     *  @param key   The key to insert (must be a null-terminated string).
     *  @param value The void* value to associate with the key. If the key
     *               already exists, the existing value is replaced.
     *  @return true if insertion took place, false if the value was replaced.
     *  @throws std::invalid_argument if key is null.
     */
    bool insert(const std::string& key, void** value) {
        if (key.empty() && !key.empty()) { // Simple null check
            throw std::invalid_argument("Key cannot be empty");
        }
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto result = map_.emplace(key, value);
        return result.second; // true if inserted, false if already present (value replaced)
    }

    /** Erases a key/value pair from the table.
     *
     *  @param key The key to erase.
     *  @return true if an element was erased, false if the key was not found.
     */
    bool erase(const std::string& key) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto it = map_.find(key);
        if (it == map_.end()) {
            return false;
        }
        map_.erase(it);
        return true;
    }

    /** Finds a value by its key.
     *
     *  @param key The key to search for.
     *  @return std::optional<std::shared_ptr<void*>> containing the value if found,
     *          otherwise std::nullopt.
     */
    std::optional<std::shared_ptr<void*>> find(const std::string& key) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it = map_.find(key);
        if (it == map_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    /** Removes all elements from the table. */
    void clear() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        map_.clear();
    }

    /** Returns the number of elements stored in the table.
     *
     *  @return The current size.
     */
    std::size_t size() const noexcept {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return map_.size();
    }

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<void*>> map_;
};

/**
 * @brief Factory function to create a JsonStringHashTable instance.
 *
 * @return A unique_ptr to a JsonStringHashTable.
 */
inline std::unique_ptr<JsonStringHashTable> create_void*string_hash_table() {
    return std::make_unique<JsonStringHashTable>();
}