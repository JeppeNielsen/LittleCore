//
// Created by Jeppe Nielsen on 03/03/2024.
//

#pragma once
#include <string>
#include <unordered_map>

namespace LittleCore {
    class ResourcePathMapper {
    public:
        using Map = std::unordered_map<std::string, std::string>;
        using GuidToPath = Map;
        using PathToGuid = Map;

        using GuidToHash = Map;

        void RefreshFromRootPath(const std::string& rootPath);

        std::string GetPath(const std::string& id) const;
        std::string GetGuid(const std::string& path) const;

        const PathToGuid& PathToGuids() const;
        const GuidToPath& GuidToPaths() const;

    private:

        GuidToPath guidToPath;
        PathToGuid pathToGuid;

    };

}
