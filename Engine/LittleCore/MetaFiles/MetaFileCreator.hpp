//
// Created by Jeppe Nielsen on 08/03/2024.
//

#pragma once
#include <string>

namespace LittleCore {

    class MetaFileCreator {
    public:
        std::string CreateMetaFile(const std::string& path);
        bool TryGetMetaGuid(const std::string& path, std::string& guid);
        std::string GetMetaPath(const std::string& path);

    };






}