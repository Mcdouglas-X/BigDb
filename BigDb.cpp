#include <pthread.h>
#include <atomic>
#include <random>
#include <chrono>
#include "secp256k1/SECP256K1.h"
#include "bloom/bloom.h"
#include "hashtable/HT.h"
#include "utils.h"
#include "secp256k1/Random.h"
#include <iostream>
#include <fstream>
#include <mutex>

std::mutex bloomMutex;
std::mutex hashTableMutex;

void run(Secp256K1& secp, HashTable& ht, BloomFilter& bloom, const std::string& targetF, int htSize);
void run(Secp256K1& secp, HashTable& ht, BloomFilter& bloom, const std::string& targetF, int htSize, std::mt19937& rng);

struct RThreadArgs {
    Secp256K1* secp;
    HashTable* ht;
    BloomFilter* bloom;
    std::string targetF;
    int htSize;
    std::mt19937 rng;
};

void* runTf(void* args) {
    RThreadArgs* ThreadArgs = static_cast<RThreadArgs*>(args);
    run(*ThreadArgs->secp, *ThreadArgs->ht, *ThreadArgs->bloom, ThreadArgs->targetF, ThreadArgs->htSize, ThreadArgs->rng);
    pthread_exit(nullptr);
}

void run(Secp256K1& secp, HashTable& ht, BloomFilter& bloom, const std::string& targetF, int htSize) {
    std::random_device rd;
    std::mt19937 rng(rd());
    run(secp, ht, bloom, targetF, htSize, rng);
}

void run(Secp256K1& secp, HashTable& ht, BloomFilter& bloom, const std::string& targetF, int htSize, std::mt19937& rng) {
    PTF(targetF, htSize, secp, ht);
    static bool bloomLoaded = false;
    
    if (!bloomLoaded && fileExists("BabyStep.bf")) {
        try {
            bloom.load("BabyStep.bf");
            bloomLoaded = true;
            std::cout << "BloomFilter loaded successfully." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Could not load the BloomFilter: " << e.what() << std::endl;
        }
    }

    std::ifstream targetFStream(targetF);
    std::string rangeL, TargetHex;
    std::getline(targetFStream, rangeL);
    std::getline(targetFStream, TargetHex);
    targetFStream.close();

    size_t colonPos = rangeL.find(':');
    std::string startHex = rangeL.substr(0, colonPos);
    std::string endHex = rangeL.substr(colonPos + 1);

    long stVal = std::stol(startHex, nullptr, 16);
    long endValue = std::stol(endHex, nullptr, 16);

    int baby_v = getLEV();
    auto elements = ht.getAllElements();

    {
        std::lock_guard<std::mutex> hashGuard(hashTableMutex);
        
    }

    std::uniform_int_distribution<long> dist(stVal, endValue);
    std::cout << "HashTable created successfully." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    while (true) {
        long randomValue = dist(rng);
        Int pk_r;
        pk_r.SetBase10(const_cast<char*>(std::to_string(randomValue).c_str()));
        Point pk_p = secp.ComputePublicKey(&pk_r);

        for (const auto& entry : elements) {
            if (entry.first.empty()) continue;

            Point hashPoint;
            bool isCompressed;
            if (!secp.ParsePublicKeyHex(entry.first, hashPoint, isCompressed)) continue;

            int pk_t;
            {
                std::lock_guard<std::mutex> hashGuard(hashTableMutex);
                pk_t = ht.get_position(entry.first);
                if (pk_t < 0) continue;
            }

            Point subResult = secp.Subtract(pk_p, hashPoint);
            std::string Hc_s = secp.GetPublicKeyHex(true, subResult);

            {
                std::lock_guard<std::mutex> bloomGuard(bloomMutex);
                uint64_t result = bloom.contains(Hc_s);
                if (result != 0) {
                    auto end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> duration = end - start;

                    int pk_mid = baby_v * pk_t;
                    long pk_end_value = (randomValue - result) + pk_mid;
                    Int pk_end;
                    pk_end.SetBase10(const_cast<char*>(std::to_string(pk_end_value).c_str()));
                    Point PUB_end = secp.ComputePublicKey(&pk_end);

                    std::ofstream outFile("found_keys.txt", std::ios::app);
                    if (outFile.is_open()) {
                        outFile << "PrivateKey found: " << pk_end.GetBase10() << "\n";
                        outFile << "PublicKey: " << secp.GetPublicKeyHex(true, PUB_end) << "\n";
                        outFile << "Time to find key: " << duration.count() << " seconds\n";
                        outFile.close();
                    }
                    std::cout << "Match found!" << std::endl;
                    return;
                }
            }
        }
    }
}
