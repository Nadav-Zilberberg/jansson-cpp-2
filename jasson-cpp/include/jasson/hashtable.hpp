#ifndef JASSON_HASHTABLE_HPP
#define JASSON_HASHTABLE_HPP

#include <unordered_map>
#include <memory>
#include <string>
#include <stdexcept>

namespace jasson {

/**
 * Forward declaration of Value for shared_ptr usage.
 */
class Value;
using ValuePtr = std::shared_ptr<Value>;

class HashTable {
public:
    using iterator = std::unordered_map<std::string, ValuePtr>::iterator;
    using const_iterator = std::unordered_map<std::string, ValuePtr>::const_iterator;

    HashTable() = default;
    ~HashTable() = default;

    // Delete copy semantics
    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    // Allow move semantics
    HashTable(HashTable&&) = default;
    HashTable& operator=(HashTable&&) = default;

    /**
     * Insert or assign a value.
     *
     * @param key The key to insert or update.
     * @param value The value to store (must be non-null).
     * @throws std::invalid_argument if key is empty or value is null.
     */
    void set(const std::string& key, ValuePtr value);

    /**
     * Retrieve a value by key.
     *
     * @param key The key to look up.
     * @return Shared pointer to the value if found.
     * @throws std::out_of_range if key is not present.
     */
    ValuePtr get(const std::string& key) const;

    /**
     * Remove a key/value pair.
     *
     * @param key The key to remove.
     * @return true if the key was found and removed, false otherwise.
     */
    bool del(const std::string& key);

    /**
     * Remove all entries from the hash table.
     */
    void clear();

    /**
     * Get the number of stored elements.
     */
    size_t size() const noexcept;

    /**
     * Check if the hash table is empty.
     */
    bool empty() const noexcept;

    /**
     * Begin iterator (non-const).
     */
    iterator begin() noexcept;

    /**
     * End iterator (non-const).
     */
    iterator end() noexcept;

    /**
     * Begin iterator (const).
     */
    const_iterator begin() const noexcept;

    /**
     * End iterator (const).
     */
    const_iterator end() const noexcept;

    /**
     * Find a key.
     *
     * @param key The key to find.
     * @return Iterator to the element if found, otherwise end().
     */
    const_iterator find(const std::string& key) const noexcept;

private:
    std::unordered_map<std::string, ValuePtr> store_;
};

} // namespace jasson

#endif // JASSON_HASHTABLE_HPP
