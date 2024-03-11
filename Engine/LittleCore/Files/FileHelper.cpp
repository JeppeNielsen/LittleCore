//
// Created by Jeppe Nielsen on 12/02/2024.
//

#include "FileHelper.hpp"
#include <fstream>
#include <vector>
#include <filesystem>

using namespace LittleCore;

std::string LittleCore::FileHelper::ReadAllText(const std::string &path) {

    std::ifstream ifs(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);

    return std::string(bytes.data(), fileSize);
}

std::vector<unsigned char> LittleCore::FileHelper::ReadData(const std::string &path) {
    std::ifstream input( path.c_str(), std::ios::binary );
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
    return buffer;
}

bool FileHelper::TryWriteAllText(const std::string &path, std::string &text) {

    std::ofstream file(path);

    if (!file.is_open()) {
        return false;
    }

    file << text;
    file.close();

    return true;
}

bool FileHelper::TryDeleteFile(const std::string &path) {
    return std::remove(path.c_str())==0;
}

bool FileHelper::FileExists(const std::string &path) {
    return std::filesystem::exists(path);
}

void FileHelper::IterateFilesRecursively(const std::string &path, const std::function<void(const std::string &)>& func) {
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(path)) {
        func(dir_entry.path());
    }
}


