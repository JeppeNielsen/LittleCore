//
// Created by Jeppe Nielsen on 12/02/2024.
//

#pragma once
#include <string>

namespace LittleCore {

    class FileHelper {
    public:
        static std::string ReadAllText(const std::string& path);
        static bool TryWriteAllText(const std::string& path, std::string& text);
        static bool TryDeleteFile(const std::string& path);

    };

}