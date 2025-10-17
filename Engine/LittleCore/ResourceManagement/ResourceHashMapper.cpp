//
// Created by Jeppe Nielsen on 14/10/2025.
//

#include "ResourceHashMapper.hpp"
#include "FileHasher.hpp"

using namespace LittleCore;

void ResourceHashMapper::IterateChangedPaths(const ResourcePathMapper& resourcePathMapper,
                                             const std::function<void(const std::string&)>& changedGuids) {
    FileHasher fileHasher;
    for(auto&[guid, path] : resourcePathMapper.GuidToPaths()) {
        auto currentHash = fileHasher.HashPath(path);
        auto& prevHash = guidToHash[guid];
        if (currentHash!=prevHash) {
            guidToHash[guid] = currentHash;
            changedGuids(guid);
        }
    }
}
