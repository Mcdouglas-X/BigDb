#ifndef HT_H
#define HT_H

#include <unordered_map>
#include <string>
#include <fstream>
#include <stdexcept>

class HashTable {
public:
    HashTable();
    void add(const std::string& item, int position);
    int get_position(const std::string& item) const;
    void save(const std::string& filename) const;
    void load(const std::string& filename);
    void reset();
    std::unordered_map<std::string, int> getAllElements() const;

private:
    std::unordered_map<std::string, int> table;
};

#endif // HT_H
