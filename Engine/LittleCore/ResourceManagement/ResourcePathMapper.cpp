//
// Created by Jeppe Nielsen on 03/03/2024.
//

#include "ResourcePathMapper.hpp"
#include "FileHelper.hpp"
#include "MetaFileCreator.hpp"

using namespace LittleCore;

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void ResourcePathMapper::RefreshFromRootPath(const std::string& rootPath) {
    guidToPath.clear();
    pathToGuid.clear();
    MetaFileCreator metaFileCreator;
    FileHelper::IterateFilesRecursively(rootPath, [&, this](const std::string& path) {
        if (ends_with(path, ".meta")) {
            return;
        }
        std::string guid;
        if (!metaFileCreator.TryGetMetaGuid(path, guid)) {
            guid = metaFileCreator.CreateMetaFile(path);
        }
        guidToPath[guid] = path;
        pathToGuid[path] = guid;
    });
}

std::string ResourcePathMapper::GetPath(const std::string& guid) const {
    auto it = guidToPath.find(guid);
    return it == guidToPath.end() ? "" : it->second;
}

std::string ResourcePathMapper::GetGuid(const std::string& path) const {
    auto it = pathToGuid.find(path);
    return it == pathToGuid.end() ? "" : it->second;
}


