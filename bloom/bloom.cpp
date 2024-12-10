// bloom.cpp
#include "bloom.h"
#include <fstream>
#include <sstream>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <stdexcept>

BloomFilter::BloomFilter(size_t size, size_t numhash)
    : size(size), numhash(numhash), filter(size) {}

std::vector<size_t> BloomFilter::hashes(const std::string& element) const {
    std::vector<size_t> hash_values;
    unsigned char hash1[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)element.c_str(), element.size(), hash1);

    unsigned char hash2[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)element.c_str(), element.size(), hash2);

    for (size_t i = 0; i < numhash; ++i) {
        size_t combined_hash = *(size_t*)(hash1 + i % SHA256_DIGEST_LENGTH) + i * (*(size_t*)(hash2 + i % MD5_DIGEST_LENGTH));
        hash_values.push_back(combined_hash % size);
    }
    return hash_values;
}

void BloomFilter::add(const std::string& element, uint64_t number) {
    auto positions = hashes(element);
    for (auto pos : positions) {
        filter[pos] = true;
    }
    data[element] = number;
}

uint64_t BloomFilter::contains(const std::string& element) const {
    auto positions = hashes(element);
    for (auto pos : positions) {
        if (!filter[pos]) {
            return 0;
        }
    }
    auto it = data.find(element);
    return it != data.end() ? it->second : 0;
}

void BloomFilter::save(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot open the file to save");
    }
    out.write((char*)&size, sizeof(size));
    out.write((char*)&numhash, sizeof(numhash));
    size_t filter_size = filter.size();
    out.write((char*)&filter_size, sizeof(filter_size));
    for (size_t i = 0; i < filter_size; ++i) {
        bool bit = filter[i];
        out.write((char*)&bit, sizeof(bit));
    }
    size_t data_size = data.size();
    out.write((char*)&data_size, sizeof(data_size));
    for (const auto& [key, value] : data) {
        size_t key_size = key.size();
        out.write((char*)&key_size, sizeof(key_size));
        out.write(key.data(), key_size);
        out.write((char*)&value, sizeof(value));
    }
    out.close();
}

void BloomFilter::load(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open the file to load");
    }
    in.read((char*)&size, sizeof(size));
    in.read((char*)&numhash, sizeof(numhash));
    size_t filter_size;
    in.read((char*)&filter_size, sizeof(filter_size));
    filter.resize(filter_size);
    for (size_t i = 0; i < filter_size; ++i) {
        bool bit;
        in.read((char*)&bit, sizeof(bit));
        filter[i] = bit;
    }
    size_t data_size;
    in.read((char*)&data_size, sizeof(data_size));
    data.clear();
    for (size_t i = 0; i < data_size; ++i) {
        size_t key_size;
        in.read((char*)&key_size, sizeof(key_size));
        std::string key(key_size, ' ');
        in.read(&key[0], key_size);
        uint64_t value;
        in.read((char*)&value, sizeof(value));
        data[key] = value;
    }
    in.close();
}
