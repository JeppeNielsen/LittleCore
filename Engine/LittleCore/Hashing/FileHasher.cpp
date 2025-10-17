//
// Created by Jeppe Nielsen on 14/10/2025.
//
#include <fstream>
#include "FileHasher.hpp"
#include "sha256.h"
#include "FileHelper.hpp"

using namespace LittleCore;

std::string FileHasher::HashPath(const std::string& path, size_t chunkSize) {
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file)
        throw std::runtime_error("Cannot open file: " + path);

    SHA256 sha;
    std::vector<char> buffer(chunkSize);

    while (file.good()) {
        file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        std::streamsize bytesRead = file.gcount();
        if (bytesRead > 0)
            sha.add(buffer.data(), static_cast<size_t>(bytesRead));
    }

    return sha.getHash();
}
