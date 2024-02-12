//
// Created by Jeppe Nielsen on 12/02/2024.
//

#include "FileReader.hpp"
#include <fstream>
#include <vector>

using namespace std;

std::string LittleCore::FileReader::ReadAllText(const std::string &path) {

    ifstream ifs(path.c_str(), ios::in | ios::binary | ios::ate);

    ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, ios::beg);

    vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);

    return string(bytes.data(), fileSize);
}
