//
// Created by Jeppe Nielsen on 08/03/2024.
//

#include "MetaFileCreator.hpp"
#include "FileHelper.hpp"
#include "GuidGenerator.hpp"

using namespace LittleCore;

GuidGenerator guidGenerator;

std::string MetaFileCreator::CreateMetaFile(const std::string &path) {
    auto metaPath = GetMetaPath(path);
    auto guid = guidGenerator.GenerateNew();
    FileHelper::TryWriteAllText(metaPath, guid);
    return guid;
}

bool MetaFileCreator::TryGetMetaGuid(const std::string &path, std::string &guid) {
    auto metaPath = GetMetaPath(path);
    if (!FileHelper::FileExists(metaPath)) {
        return false;
    }
    guid = FileHelper::ReadAllText(metaPath);
    return true;
}

std::string MetaFileCreator::GetMetaPath(const std::string &path) {
    return path + ".meta";
}
