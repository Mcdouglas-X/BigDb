#include "utils.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "secp256k1/SECP256K1.h"

int getLEV() {
    std::ifstream logFile("log.txt");
    int lastEnd = 0;
    if (logFile.is_open()) {
        logFile >> lastEnd;
        logFile.close();
    }
    return lastEnd;
}

bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

void PTF(const std::string& filename, int htSize, Secp256K1& secp, HashTable& ht) {
    std::ifstream targetF(filename);
    if (!targetF.is_open()) {
        throw std::runtime_error("Unable to open target file");
    }
    std::string rangeL, TargetHex;
    std::getline(targetF, rangeL);
    std::getline(targetF, TargetHex);
    targetF.close();

    size_t colonPos = rangeL.find(':');
    if (colonPos == std::string::npos) {
        throw std::runtime_error("Invalid range format in target file");
    }

    std::string startHex = rangeL.substr(0, colonPos);
    std::string endHex = rangeL.substr(colonPos + 1);

    Int privKey;
    privKey.SetBase10(const_cast<char*>(std::to_string(getLEV()).c_str()));
    Point pubKey = secp.ComputePublicKey(&privKey);

    Point targetPub;
    bool isCompressed;
    if (!secp.ParsePublicKeyHex(TargetHex, targetPub, isCompressed)) {
        throw std::runtime_error("Invalid public key format in target file");
    }

    std::vector<Point> subR = secp.SubtractMultiple(targetPub, pubKey, htSize);
    for (int i = 0; i < subR.size(); ++i) {
        std::string resultKH = secp.GetPublicKeyHex(true, subR[i]);
        ht.add(resultKH, i + 1);
    }
}
