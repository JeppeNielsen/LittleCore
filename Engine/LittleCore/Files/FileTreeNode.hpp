//
// Created by Jeppe Nielsen on 07/01/2026.
//

#pragma once
#include <string>
#include <map>

namespace LittleCore {
    struct FileTreeNode {
        std::string name;
        std::string path;
        std::string fullPath;
        std::map<std::string, FileTreeNode> children;
        FileTreeNode* parent;
        bool isFile = false;
    };
}