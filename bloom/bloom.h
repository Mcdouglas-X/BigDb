// bloom.h
#ifndef BLOOM_H
#define BLOOM_H

#include <vector>
#include <unordered_map>
#include <string>

class BloomFilter {
public:
    BloomFilter(size_t size, size_t numhash);
    void add(const std::string& element, uint64_t number);
    uint64_t contains(const std::string& element) const;
    void save(const std::string& filename) const;
    void load(const std::string& filename);

private:
    size_t size;
    size_t numhash;
    std::vector<bool> filter;
    std::unordered_map<std::string, uint64_t> data;

    std::vector<size_t> hashes(const std::string& element) const;
};

#endif // BLOOM_H
