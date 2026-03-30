//
// Created by Jeppe Nielsen on 30/03/2026.
//

#pragma once
#include "FileTreeNode.hpp"
#include <string>
#include <vector>

class CodeEditorProjectWindow {
public:
    struct DrawResult {
        bool refreshed = false;
        std::string openedPath;
    };

    explicit CodeEditorProjectWindow(std::string rootPath = {});

    void SetRootPath(std::string rootPath);
    const std::string& RootPath() const;
    const std::vector<std::string>& CodeFiles() const;

    void Refresh();
    DrawResult Draw(const std::string& activePath);

private:
    std::string rootPath;
    std::vector<std::string> codeFiles;
    LittleCore::FileTreeNode treeRoot;

    void RebuildTree();
    void DrawNode(const LittleCore::FileTreeNode& node, const std::string& activePath, DrawResult& result);
};
