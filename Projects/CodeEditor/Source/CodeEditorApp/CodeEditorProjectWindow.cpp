//
// Created by Jeppe Nielsen on 30/03/2026.
//

#include "CodeEditorProjectWindow.hpp"
#include "FileTreeFactory.hpp"
#include "imgui.h"
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <utility>

using namespace LittleCore;

namespace {
    bool IsCodeFile(const std::filesystem::path& path) {
        const auto extension = path.extension().string();
        return extension == ".cpp" || extension == ".hpp";
    }

    bool ShouldSkipPath(const std::filesystem::path& rootPath, const std::filesystem::path& path) {
        std::error_code errorCode;
        const auto relativePath = std::filesystem::relative(path, rootPath, errorCode);
        if (errorCode) {
            return false;
        }

        for (const auto& part : relativePath) {
            const auto name = part.string();
            if (name == "Build" || name == "Cache" || name == ".git" || name == ".idea") {
                return true;
            }

            if (!name.empty() && name[0] == '.') {
                return true;
            }
        }

        return false;
    }
}

CodeEditorProjectWindow::CodeEditorProjectWindow(std::string rootPath) : rootPath(std::move(rootPath)) {
}

void CodeEditorProjectWindow::SetRootPath(std::string value) {
    rootPath = std::move(value);
}

const std::string& CodeEditorProjectWindow::RootPath() const {
    return rootPath;
}

const std::vector<std::string>& CodeEditorProjectWindow::CodeFiles() const {
    return codeFiles;
}

void CodeEditorProjectWindow::Refresh() {
    codeFiles.clear();
    treeRoot = {};
    treeRoot.name = "Project";
    treeRoot.fullPath = rootPath;

    if (rootPath.empty() || !std::filesystem::exists(rootPath)) {
        return;
    }

    std::error_code errorCode;
    for (std::filesystem::recursive_directory_iterator iterator(rootPath, errorCode), end; iterator != end; iterator.increment(errorCode)) {
        if (errorCode) {
            errorCode.clear();
            continue;
        }

        const auto& entry = *iterator;
        if (ShouldSkipPath(rootPath, entry.path())) {
            if (entry.is_directory()) {
                iterator.disable_recursion_pending();
            }
            continue;
        }

        if (!entry.is_regular_file() || !IsCodeFile(entry.path())) {
            continue;
        }

        codeFiles.push_back(entry.path().lexically_normal().generic_string());
    }

    std::sort(codeFiles.begin(), codeFiles.end());
    RebuildTree();
}

void CodeEditorProjectWindow::RebuildTree() {
    std::unordered_map<std::string, std::string> filePaths;
    for (const auto& path : codeFiles) {
        filePaths.insert({path, path});
    }

    FileTreeFactory fileTreeFactory;
    fileTreeFactory.CreateTree(treeRoot, filePaths, rootPath);
}

CodeEditorProjectWindow::DrawResult CodeEditorProjectWindow::Draw(const std::string& activePath) {
    DrawResult result;

    ImGui::Begin("Files");

    if (ImGui::Button("Refresh")) {
        Refresh();
        result.refreshed = true;
    }

    ImGui::SameLine();
    ImGui::TextWrapped("%s", rootPath.c_str());

    if (treeRoot.children.empty()) {
        ImGui::TextWrapped("No .cpp or .hpp files were found under the selected project root.");
    } else {
        DrawNode(treeRoot, activePath, result);
    }

    ImGui::End();
    return result;
}

void CodeEditorProjectWindow::DrawNode(const FileTreeNode& node, const std::string& activePath, DrawResult& result) {
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (node.children.empty()) {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (node.isFile && node.fullPath == activePath) {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    if (node.name == "Project") {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    }

    const bool isOpen = ImGui::TreeNodeEx(node.name.c_str(), nodeFlags);
    if (node.isFile && ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
        result.openedPath = node.fullPath;
    }

    if (isOpen) {
        for (const auto& [name, child] : node.children) {
            (void)name;
            DrawNode(child, activePath, result);
        }
        ImGui::TreePop();
    }
}
