#ifndef JSSON_CPP_HASHTABLE_IMPL_HPP
#define JSSON_CPP_HASHTABLE_IMPL_HPP

template <typename K, typename V>
bool HashTable<K, V>::insert(const K& key, const V& value) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto result = map_.emplace(key, value);
    return result.second; // true if inserted, false if already present (value replaced)
}

template <typename K, typename V>
bool HashTable<K, V>::erase(const K& key) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto it = map_.find(key);
    if (it == map_.end()) {
        return false;
    }
    map_.erase(it);
    return true;
}

template <typename K, typename V>
std::optional<V> HashTable<K, V>::find(const K& key) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto it = map_.find(key);
    if (it == map_.end()) {
        return std::nullopt;
    }
    return it->second;
}

template <typename K, typename V>
void HashTable<K, V>::clear() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    map_.clear();
}

template <typename K, typename V>
std::size_t HashTable<K, V>::size() const noexcept {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return map_.size();
}

#endif // JSSON_CPP_HASHTABLE_IMPL_HPP