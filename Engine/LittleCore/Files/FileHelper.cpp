//
// Created by Jeppe Nielsen on 12/02/2024.
//

#include "FileHelper.hpp"
#include <fstream>
#include <vector>

using namespace std;

std::string LittleCore::FileHelper::ReadAllText(const std::string &path) {

    ifstream ifs(path.c_str(), ios::in | ios::binary | ios::ate);

    ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, ios::beg);

    vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);

    return string(bytes.data(), fileSize);
}

bool LittleCore::FileHelper::TryWriteAllText(const string &path, string &text) {

    ofstream file(path);

    if (!file.is_open()) {
        return false;
    }

    file << text;
    file.close();

    return true;
}

bool LittleCore::FileHelper::TryDeleteFile(const string &path) {
    return std::remove(path.c_str())==0;
}
