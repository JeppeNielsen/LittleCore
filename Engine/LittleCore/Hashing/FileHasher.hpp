//
// Created by Jeppe Nielsen on 14/10/2025.
//


#pragma once
#include <string>

namespace LittleCore {
    struct FileHasher {

        std::string HashPath(const std::string& path, size_t chunkSize = 8 * 1024 * 1024);

    };
}