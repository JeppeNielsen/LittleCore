//
// Created by Jeppe Nielsen on 12/02/2024.
//

#pragma once
#include <string>
#include <vector>
#include <functional>

namespace LittleCore {

    class FileHelper {
    public:
        static std::string ReadAllText(const std::string& path);
        static std::vector<unsigned char> ReadData(const std::string& path);
        static bool TryWriteAllText(const std::string& path, std::string& text);
        static bool TryDeleteFile(const std::string& path);
        static bool FileExists(const std::string& path);
        static void IterateFilesRecursively(const std::string& path, std::function<void(const std::string& path)>& func);
    };

}