//
// Created by Jeppe Nielsen on 07/01/2026.
//

#include "FileTreeFactory.hpp"
#include <vector>
#include <sstream>

using namespace LittleCore;

std::vector<std::string> splitPath(const std::string& path, char delimiter = '/') {
    std::vector<std::string> components;
    std::stringstream ss(path);
    std::string part;
    while (std::getline(ss, part, delimiter)) {
        if (!part.empty()) {
            components.push_back(part);
        }
    }
    return components;
}

void FileTreeFactory::CreateTree(FileTreeNode& root, const std::unordered_map<std::string, std::string>& filePaths,
                                 const std::string& cutoffPath, const std::function<bool(const std::string& path)>& predicate) {


    for (const auto& pathKeyValue : filePaths) {
        if (predicate && !predicate(pathKeyValue.first)) {
            continue;
        }

        std::string path = pathKeyValue.first.substr(cutoffPath.size());
        auto components = splitPath(path);
        FileTreeNode* current = &root;
        std::string fullPath = cutoffPath;
        for (size_t i = 0; i < components.size(); ++i) {
            const auto& part = components[i];

            fullPath += "/" + part;

            if (i == components.size() - 1) {
                // Mark the last component as a file

                auto& file = current->children[part];
                file.name = part;
                file.path = path;
                file.fullPath = fullPath;
                file.isFile = true;
                file.parent = current;
            } else {
                // Navigate or create the directory node
                auto old = current;
                current = &current->children[part];
                current->name = part;
                current->path = path;
                current->fullPath = fullPath;
                current->isFile = false;
                current->parent = old;
            }
        }
    }

}
