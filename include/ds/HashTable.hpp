#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <stdexcept>
#include <cstddef>

template <typename K, typename V>
class HashTable {
public:
    struct Entry {
        K key;
        V value;
        Entry(const K& k, const V& v) : key(k), value(v) {}
    };

private:
    std::vector<std::vector<Entry>> buckets;
    size_t numElements;
    size_t numBuckets;
    float maxLoadFactor;

    size_t hashKey(const K& key) const {
        return getHash(key) % numBuckets;
    }

    // Custom hash specializations
    template <typename T>
    size_t getHash(const T& key) const {
        return std::hash<T>{}(key);
    }

    size_t getHash(const std::string& key) const {
        // Polynomial rolling hash function
        size_t hashVal = 0;
        size_t p = 31;
        size_t m = 1e9 + 9;
        size_t p_pow = 1;
        for (char c : key) {
            hashVal = (hashVal + (c - 'a' + 1) * p_pow) % m;
            p_pow = (p_pow * p) % m;
        }
        return hashVal;
    }

    void rehash() {
        size_t oldBuckets = numBuckets;
        numBuckets *= 2;
        std::vector<std::vector<Entry>> newBuckets(numBuckets);

        for (size_t i = 0; i < oldBuckets; ++i) {
            for (const auto& entry : buckets[i]) {
                size_t newIndex = getHash(entry.key) % numBuckets;
                newBuckets[newIndex].push_back(entry);
            }
        }
        buckets = std::move(newBuckets);
    }

public:
    HashTable(size_t initialCapacity = 16, float loadFactor = 0.75f)
        : numElements(0), numBuckets(initialCapacity), maxLoadFactor(loadFactor) {
        buckets.resize(numBuckets);
    }

    ~HashTable() = default;

    void put(const K& key, const V& value) {
        if ((float)(numElements + 1) / numBuckets > maxLoadFactor) {
            rehash();
        }

        size_t index = hashKey(key);
        for (auto& entry : buckets[index]) {
            if (entry.key == key) {
                entry.value = value;
                return;
            }
        }

        buckets[index].emplace_back(key, value);
        numElements++;
    }

    V* get(const K& key) {
        size_t index = hashKey(key);
        for (auto& entry : buckets[index]) {
            if (entry.key == key) {
                return &(entry.value);
            }
        }
        return nullptr;
    }

    const V* get(const K& key) const {
        size_t index = hashKey(key);
        for (const auto& entry : buckets[index]) {
            if (entry.key == key) {
                return &(entry.value);
            }
        }
        return nullptr;
    }

    bool contains(const K& key) const {
        return get(key) != nullptr;
    }

    bool remove(const K& key) {
        size_t index = hashKey(key);
        auto& chain = buckets[index];
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if (it->key == key) {
                chain.erase(it);
                numElements--;
                return true;
            }
        }
        return false;
    }

    size_t size() const {
        return numElements;
    }

    bool empty() const {
        return numElements == 0;
    }

    size_t bucketCount() const {
        return numBuckets;
    }

    float currentLoadFactor() const {
        return (float)numElements / numBuckets;
    }

    std::vector<K> keys() const {
        std::vector<K> keyList;
        keyList.reserve(numElements);
        for (const auto& chain : buckets) {
            for (const auto& entry : chain) {
                keyList.push_back(entry.key);
            }
        }
        return keyList;
    }

    std::vector<std::pair<K, V>> entries() const {
        std::vector<std::pair<K, V>> result;
        result.reserve(numElements);
        for (const auto& chain : buckets) {
            for (const auto& entry : chain) {
                result.push_back({entry.key, entry.value});
            }
        }
        return result;
    }

    void clear() {
        buckets.clear();
        buckets.resize(16);
        numBuckets = 16;
        numElements = 0;
    }
};

#endif // HASH_TABLE_HPP
