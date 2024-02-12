//
// Created by Jeppe Nielsen on 12/02/2024.
//

#pragma once
#include <string>

namespace LittleCore {

    class FileReader {
    public:
        static std::string ReadAllText(const std::string& path);

    };

}