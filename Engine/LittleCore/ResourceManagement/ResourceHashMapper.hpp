//
// Created by Jeppe Nielsen on 14/10/2025.
//


#pragma once
#include <unordered_map>
#include <string>
#include "ResourcePathMapper.hpp"
#include <functional>

namespace LittleCore {
    class ResourceHashMapper {
        using GuidToHash = std::unordered_map<std::string, std::string>;
        GuidToHash guidToHash;
    public:
        void IterateChangedPaths(const ResourcePathMapper& resourcePathMapper, const std::function<void(const std::string&)>& changedGuids);

    };

}
