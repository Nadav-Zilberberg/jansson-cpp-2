#ifndef JSSON_CPP_HASHTABLE_HPP
#define JSSON_CPP_HASHTABLE_HPP

#include <unordered_map>
#include <optional>
#include <shared_mutex>

/**
 * @brief A thread-safe hash table implementation using modern C++.
 *
 * @tparam K The key type.
 * @tparam V The value type.
 */
template <typename K, typename V>
class HashTable {
public:
    /** Constructs an empty hash table. */
    HashTable() = default;

    /** Destroys the hash table. */
    ~HashTable() = default;

    /** Inserts or assigns a value for a given key.
     *
     *  @param key   The key to insert.
     *  @param value The value to associate with the key.
     *  @return true if insertion took place, false if value was replaced.
     */
    bool insert(const K& key, const V& value);

    /** Erases a key/value pair from the table.
     *
     *  @param key The key to erase.
     *  @return true if an element was erased, false if the key was not found.
     */
    bool erase(const K& key);

    /** Finds a value by its key.
     *
     *  @param key The key to search for.
     *  @return std::optional<V> containing the value if found, otherwise std::nullopt.
     */
    std::optional<V> find(const K& key) const;

    /** Removes all elements from the table. */
    void clear();

    /** Returns the number of elements stored in the table.
     *
     *  @return The current size.
     */
    std::size_t size() const noexcept;

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<K, V> map_;
};

#include "hashtable_impl.hpp"

#endif // JSSON_CPP_HASHTABLE_HPP