#include "secp256k1/SECP256K1.h"
#include "bloom/bloom.h"
#include "utils.h"
#include <iostream>
#include <pthread.h>
#include <vector>
#include <string>
#include <mutex>
#include <fstream>
#include <atomic>

extern std::mutex bloomMutex; 
std::atomic<int> progress(0);

struct ThreadArgs {
    Secp256K1* secp;
    BloomFilter* bloom;
    int stVal;
    int endValue;
    std::string filename;
    std::atomic<int>* count;
    int total;
};

void printProgress(int total) {
    int barWidth = 70;
    std::cout << "[";
    int pos = barWidth * progress / total;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress / (double)total * 100.0) << " %\r";
    std::cout.flush();
}

void processRange(Secp256K1& secp, BloomFilter& bloom, int stVal, int endValue, const std::string& filename, std::atomic<int>& count, int total) {
    for (int i = stVal; i <= endValue; ++i) {
        Int privKey;
        privKey.SetBase10(const_cast<char*>(std::to_string(i).c_str()));
        Point pubKey = secp.ComputePublicKey(&privKey);
        std::string pubKeyHex = secp.GetPublicKeyHex(true, pubKey);
        {
            std::lock_guard<std::mutex> guard(bloomMutex);
            bloom.add(pubKeyHex, i);
        }
        count++;
        progress++;
        if (count >= 1000000) {
            bloom.save(filename);
            count = 0;
        }
        if (progress % (total / 100) == 0) {
            printProgress(total);
        }
    }
}

void* threadFun(void* args) {
    ThreadArgs* threadArgs = static_cast<ThreadArgs*>(args);
    processRange(*threadArgs->secp, *threadArgs->bloom, threadArgs->stVal, threadArgs->endValue, threadArgs->filename, *threadArgs->count, threadArgs->total);
    pthread_exit(nullptr);
}

void GSKM(Secp256K1& secp, int range, const std::string& filename, int numThreads) {
    int lastEnd = 0;
    if (fileExists(filename)) {
        lastEnd = getLEV();
    }
    int stVal = lastEnd + 1;
    int endValue = lastEnd + range;
    int total = endValue - stVal + 1;
    std::atomic<int> count = 0;
    progress = 0;
    BloomFilter bloom(1000000, 7);
    if (fileExists(filename)) {
        try {
            bloom.load(filename);
        } catch (const std::exception& e) {
            std::cerr << "Could not load the existing file: " << e.what() << std::endl;
        }
    }
    std::vector<pthread_t> threads(numThreads);
    std::vector<ThreadArgs> threadArgs(numThreads);
    int RPT = total / numThreads;
    for (int i = 0; i < numThreads; ++i) {
        int start = stVal + i * RPT;
        int end = (i == numThreads - 1) ? endValue : start + RPT - 1;
        threadArgs[i] = { &secp, &bloom, start, end, filename, &count, total };
        if (pthread_create(&threads[i], nullptr, threadFun, static_cast<void*>(&threadArgs[i]))) {
            std::cerr << "Error creating thread " << i << std::endl;
            return;
        }
    }
    for (pthread_t& t : threads) {
        pthread_join(t, nullptr);
    }
    bloom.save(filename);
    std::ofstream logFile("log.txt");
    logFile << endValue;
    logFile.close();
    printProgress(total);
    std::cout << std::endl;
}
