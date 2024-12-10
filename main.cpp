#include <pthread.h>
#include <atomic>
#include <random>
#include <thread>
#include "secp256k1/SECP256K1.h"
#include "bloom/bloom.h"
#include "hashtable/HT.h"
#include "utils.h"
#include <iostream>
#include <cstdlib>
#include <string>

void run(Secp256K1& secp, HashTable& ht, BloomFilter& bloom, const std::string& targetF, int htSize, std::mt19937& rng);
void GSK(Secp256K1& secp, int range, const std::string& filename);
void GSKM(Secp256K1& secp, int range, const std::string& filename, int numThreads);

void printHelp() {
    std::cout << "Bigdb v0.1(alpha) by Mcdouglasx\n"
              << "Usage: Bigdb -bs [range] [-t [number of threads]] -f [target file] -ht [hash table size]\n"
              << "Options:\n"
              << "  -bs [range]              Generate sequential keys in the given range.\n"
              << "  -t [number of threads]   Specify the number of threads to use (default is to use all available threads minus one).\n"
              << "  -f [target file]         Specify the target file to use.\n"
              << "  -ht [hash table size]    Generate and save hash table.\n"
              << "  -h                       Display this help message.\n";
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    std::cout << "Bigdb v0.1(alpha) by Mcdouglasx\n";
    Secp256K1 secp;
    secp.Init();

    int numThreads = std::thread::hardware_concurrency() - 1;
    std::string targetF;
    int htSize = 0;

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-bs" && i + 1 < argc) {
            int range = std::stoi(argv[i + 1]);
            if (i + 3 < argc && std::string(argv[i + 2]) == "-t") {
                numThreads = std::stoi(argv[i + 3]);
            }
            GSKM(secp, range, "BabyStep.bf", numThreads);
            return 0;
        } else if (std::string(argv[i]) == "-f" && i + 1 < argc) {
            targetF = argv[i + 1];
        } else if (std::string(argv[i]) == "-ht" && i + 1 < argc) {
            htSize = std::stoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "-h") {
            printHelp();
            return 0;
        }
    }

    if (!targetF.empty() && htSize > 0) {
        HashTable ht;
        BloomFilter bloom(1000000, 7);
        std::random_device rd;
        std::mt19937 rng(rd());
        run(secp, ht, bloom, targetF, htSize, rng);
        return 0;
    }

    printHelp();
    return 0;
}
