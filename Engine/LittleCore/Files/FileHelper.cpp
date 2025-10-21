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

std::vector<uint8_t> LittleCore::FileHelper::ReadData(const std::string &path) {
    std::ifstream input( path.c_str(), std::ios::binary );
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(input), {});
    return buffer;
}
/*
std::vector<uint8_t> readFile(const char* filename)
{
    // open the file:
    std::ifstream file(filename, std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<uint8_t> vec;
    vec.reserve(fileSize);

    // read the data:
    vec.insert(vec.begin(),
               std::istream_iterator<uint8_t>(file),
               std::istream_iterator<uint8_t>());

    return vec;
}
*/

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

std::string FileHelper::GetExtension(const std::string &path) {
    return std::filesystem::path(path).extension().string();
}

std::string FileHelper::GetFilenameWithoutExtension(const std::string &path) {
    return std::filesystem::path(path).filename();
}

bool FileHelper::HasExtension(const std::string& path, const std::string& extension) {
    if (path.length() >= extension.length()) {
        return (0 == path.compare(path.length() - extension.length(), extension.length(), extension));
    }
    return false;
}
