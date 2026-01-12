//
// Created by Jeppe Nielsen on 07/01/2026.
//


#pragma once
#include "FileTreeNode.hpp"
#include <unordered_map>
#include <functional>

namespace LittleCore {
    class FileTreeFactory {
    public:

        void CreateTree(FileTreeNode& root, const std::unordered_map<std::string, std::string>& filePaths, const std::string& cutoffPath, const std::function<bool(const std::string& path)>& predicate = nullptr);

    };

}
