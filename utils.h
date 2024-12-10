#ifndef UTILS_H
#define UTILS_H

#include "secp256k1/SECP256K1.h"
#include "hashtable/HT.h"
#include "bloom/bloom.h"
#include <string>
#include <mutex>
#include <random>

int getLEV();
bool fileExists(const std::string& filename);
void PTF(const std::string& filename, int htSize, Secp256K1& secp, HashTable& ht);
void run(Secp256K1& secp, HashTable& ht, BloomFilter& bloom, const std::string& targetF, int htSize);
void run(Secp256K1& secp, HashTable& ht, BloomFilter& bloom, const std::string& targetF, int htSize, std::mt19937& rng);

extern std::mutex bloomMutex;
extern std::mutex hashTableMutex;

#endif // UTILS_H
