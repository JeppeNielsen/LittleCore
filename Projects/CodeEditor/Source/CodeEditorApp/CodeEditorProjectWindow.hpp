//
// Created by Jeppe Nielsen on 30/03/2026.
//

#pragma once
#include "FileTreeNode.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

class CodeEditorProjectWindow {
public:
    struct DrawResult {
        bool refreshed = false;
        std::string openedPath;
        std::string statusText;
        std::vector<std::string> removedPaths;
    };

    explicit CodeEditorProjectWindow(std::string rootPath = {});

    void SetRootPath(std::string rootPath);
    const std::string& RootPath() const;
    const std::vector<std::string>& CodeFiles() const;

    void Refresh();
    DrawResult Draw(const std::string& activePath);

private:
    enum class CreateKind {
        File,
        Folder
    };

    struct CreateRequest {
        CreateKind kind = CreateKind::File;
        std::string parentPath;
        std::string name;
        int selectedExtensionIndex = 0;
    };

    struct DeleteRequest {
        std::string path;
        bool isDirectory = false;
    };

    struct ContextMenuTarget {
        std::string path;
        bool isFile = false;
    };

    std::string rootPath;
    std::vector<std::string> codeFiles;
    LittleCore::FileTreeNode treeRoot;
    std::optional<CreateRequest> pendingCreateRequest;
    std::optional<DeleteRequest> pendingDeleteRequest;
    std::optional<ContextMenuTarget> pendingContextMenuTarget;
    bool shouldOpenContextMenu = false;
    bool shouldOpenCreateModal = false;
    bool shouldOpenDeleteModal = false;

    void RebuildTree();
    void AddDirectoryNode(const std::string& fullPath);
    void AddFileNode(const std::string& fullPath);
    LittleCore::FileTreeNode* EnsureDirectoryNode(const std::filesystem::path& relativePath);
    void OpenCreatePopup(CreateKind kind, const std::string& parentPath);
    void OpenDeletePopup(const std::string& path, bool isDirectory);
    void DrawCreatePopup(DrawResult& result);
    void DrawDeletePopup(DrawResult& result);
    void DrawContextMenuPopup(DrawResult& result);
    void DrawNode(const LittleCore::FileTreeNode& node, const std::string& activePath, DrawResult& result);
};
