#include "HT.h"
#include <fstream>
#include <stdexcept>

HashTable::HashTable() {}

std::unordered_map<std::string, int> HashTable::getAllElements() const {
    return table;
}

void HashTable::add(const std::string& item, int position) {
    table[item] = position;
}

int HashTable::get_position(const std::string& item) const {
    auto it = table.find(item);
    return it != table.end() ? it->second : -1;
}

void HashTable::save(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) {
        throw std::runtime_error("Unable to open file for saving");
    }
    size_t size = table.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (const auto& [key, value] : table) {
        size_t key_size = key.size();
        out.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
        out.write(key.c_str(), key_size);
        out.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
    out.close();
}

void HashTable::load(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        throw std::runtime_error("Unable to open file for loading");
    }
    size_t size;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));
    table.clear();
    for (size_t i = 0; i < size; ++i) {
        size_t key_size;
        in.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
        std::string key(key_size, ' ');
        in.read(&key[0], key_size);
        int value;
        in.read(reinterpret_cast<char*>(&value), sizeof(value));
        table[key] = value;
    }
    in.close();
}

void HashTable::reset() {
    table.clear();
}
