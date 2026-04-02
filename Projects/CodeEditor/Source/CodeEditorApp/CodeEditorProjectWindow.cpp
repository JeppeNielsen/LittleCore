//
// Created by Jeppe Nielsen on 30/03/2026.
//

#include "CodeEditorProjectWindow.hpp"
#include "CodeEditorPathUtils.hpp"
#include "FileHelper.hpp"
#include "PathHelper.hpp"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include <algorithm>
#include <filesystem>
#include <string_view>
#include <utility>

using namespace LittleCore;

namespace {
    constexpr std::string_view CreateFilePopupId = "Create Source File";
    constexpr std::string_view CreateFolderPopupId = "Create Folder";
    constexpr std::string_view DeletePopupId = "Delete Item";

    constexpr const char* SourceFileExtensions[] = {
            ".cpp",
            ".hpp"
    };

    bool IsCodeFile(const std::filesystem::path& path) {
        const auto extension = path.extension().string();
        return extension == ".cpp" || extension == ".hpp";
    }

    bool IsSimplePathComponent(const std::string& value) {
        return !value.empty() &&
               value != "." &&
               value != ".." &&
               value.find('/') == std::string::npos &&
               value.find('\\') == std::string::npos;
    }

    std::string BuildSourceFileTemplate(const std::filesystem::path& path) {
        const auto extension = path.extension().string();
        if (extension == ".hpp") {
            return "#pragma once\n\n";
        }

        return "\n";
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

std::string CodeEditorProjectWindow::MakeDisplayPath(const std::string& path) const {
    return CodeEditorPathUtils::MakeDisplayPath(path, rootPath);
}

void CodeEditorProjectWindow::Refresh() {
    codeFiles.clear();
    treeRoot = {};
    treeRoot.name = "Project";
    treeRoot.fullPath = rootPath;
    treeRoot.parent = nullptr;

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

        if (entry.is_directory()) {
            AddDirectoryNode(entry.path().lexically_normal().generic_string());
            continue;
        }

        if (!entry.is_regular_file() || !IsCodeFile(entry.path())) {
            continue;
        }

        const auto fullPath = entry.path().lexically_normal().generic_string();
        codeFiles.push_back(fullPath);
        AddFileNode(fullPath);
    }

    std::sort(codeFiles.begin(), codeFiles.end());
}

void CodeEditorProjectWindow::RebuildTree() {
    treeRoot = {};
    treeRoot.name = "Project";
    treeRoot.fullPath = rootPath;
    treeRoot.parent = nullptr;

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

        if (entry.is_directory()) {
            AddDirectoryNode(entry.path().lexically_normal().generic_string());
        } else if (entry.is_regular_file() && IsCodeFile(entry.path())) {
            AddFileNode(entry.path().lexically_normal().generic_string());
        }
    }
}

FileTreeNode* CodeEditorProjectWindow::EnsureDirectoryNode(const std::filesystem::path& relativePath) {
    FileTreeNode* current = &treeRoot;
    std::filesystem::path currentPath = rootPath;

    for (const auto& part : relativePath) {
        const auto name = part.string();
        if (name.empty() || name == ".") {
            continue;
        }

        currentPath /= part;
        auto& child = current->children[name];
        child.name = name;
        child.path = CodeEditorPathUtils::NormalizePath(std::filesystem::relative(currentPath, rootPath).generic_string());
        child.fullPath = CodeEditorPathUtils::NormalizePath(currentPath.generic_string());
        child.isFile = false;
        child.parent = current;
        current = &child;
    }

    return current;
}

void CodeEditorProjectWindow::AddDirectoryNode(const std::string& fullPath) {
    std::error_code errorCode;
    const auto relativePath = std::filesystem::relative(fullPath, rootPath, errorCode);
    if (errorCode) {
        return;
    }

    (void)EnsureDirectoryNode(relativePath);
}

void CodeEditorProjectWindow::AddFileNode(const std::string& fullPath) {
    std::error_code errorCode;
    const auto relativePath = std::filesystem::relative(fullPath, rootPath, errorCode);
    if (errorCode) {
        return;
    }

    auto* directory = EnsureDirectoryNode(relativePath.parent_path());
    const auto name = std::filesystem::path(fullPath).filename().string();
    auto& file = directory->children[name];
    file.name = name;
    file.path = CodeEditorPathUtils::NormalizePath(relativePath.generic_string());
    file.fullPath = CodeEditorPathUtils::NormalizePath(fullPath);
    file.isFile = true;
    file.parent = directory;
}

void CodeEditorProjectWindow::OpenCreatePopup(CreateKind kind, const std::string& parentPath) {
    pendingCreateRequest = CreateRequest{
            .kind = kind,
            .parentPath = parentPath,
            .name = "",
            .selectedExtensionIndex = 0
    };
    shouldOpenCreateModal = true;
}

void CodeEditorProjectWindow::OpenDeletePopup(const std::string& path, bool isDirectory) {
    pendingDeleteRequest = DeleteRequest{
            .path = path,
            .isDirectory = isDirectory
    };
    shouldOpenDeleteModal = true;
}

void CodeEditorProjectWindow::DrawCreatePopup(DrawResult& result) {
    if (pendingCreateRequest.has_value()) {
        const char* popupId = pendingCreateRequest->kind == CreateKind::File ? CreateFilePopupId.data() : CreateFolderPopupId.data();
        ImGui::SetNextWindowSize(ImVec2(460.0f, 0.0f), ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal(popupId, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextWrapped("%s in:", pendingCreateRequest->kind == CreateKind::File ? "Create a new source file" : "Create a new folder");
            ImGui::Separator();
            ImGui::TextWrapped("%s", MakeDisplayPath(pendingCreateRequest->parentPath).c_str());
            ImGui::InputText("Name", &pendingCreateRequest->name);

            if (pendingCreateRequest->kind == CreateKind::File) {
                ImGui::Combo("Extension",
                             &pendingCreateRequest->selectedExtensionIndex,
                             SourceFileExtensions,
                             2);
            }

            bool closePopup = false;
            if (ImGui::Button("Create")) {
                const std::string rawName = pendingCreateRequest->name;
                const bool validName = IsSimplePathComponent(rawName);
                std::string statusText;
                if (!validName) {
                    statusText = "Name must be a single file or folder name.";
                } else {
                    std::filesystem::path targetPath = pendingCreateRequest->parentPath;
                    if (pendingCreateRequest->kind == CreateKind::Folder) {
                        targetPath /= rawName;
                    } else {
                        const auto desiredExtension = SourceFileExtensions[pendingCreateRequest->selectedExtensionIndex];
                        std::filesystem::path fileName(rawName);
                        const auto currentExtension = fileName.extension().string();
                        if (currentExtension.empty()) {
                            fileName += desiredExtension;
                        } else if (currentExtension != ".cpp" && currentExtension != ".hpp") {
                            statusText = "Source files must use .cpp or .hpp.";
                        }
                        targetPath /= fileName;
                    }

                    if (statusText.empty()) {
                        std::error_code errorCode;
                        if (std::filesystem::exists(targetPath, errorCode)) {
                            statusText = MakeDisplayPath(targetPath.generic_string()) + " already exists.";
                        } else if (pendingCreateRequest->kind == CreateKind::Folder) {
                            if (std::filesystem::create_directory(targetPath, errorCode)) {
                                Refresh();
                                result.refreshed = true;
                                statusText = "Created folder " + MakeDisplayPath(targetPath.generic_string());
                                closePopup = true;
                            } else {
                                statusText = "Failed to create folder " + MakeDisplayPath(targetPath.generic_string());
                            }
                        } else {
                            auto fileText = BuildSourceFileTemplate(targetPath);
                            if (FileHelper::TryWriteAllText(targetPath.generic_string(), fileText)) {
                                Refresh();
                                result.refreshed = true;
                                result.openedPath = CodeEditorPathUtils::NormalizePath(targetPath.generic_string());
                                statusText = "Created file " + MakeDisplayPath(targetPath.generic_string());
                                closePopup = true;
                            } else {
                                statusText = "Failed to create file " + MakeDisplayPath(targetPath.generic_string());
                            }
                        }
                    }
                }

                if (!statusText.empty()) {
                    result.statusText = std::move(statusText);
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                closePopup = true;
            }

            if (closePopup) {
                pendingCreateRequest.reset();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

void CodeEditorProjectWindow::DrawDeletePopup(DrawResult& result) {
    if (pendingDeleteRequest.has_value()) {
        ImGui::SetNextWindowSize(ImVec2(480.0f, 0.0f), ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal(DeletePopupId.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            const auto fileName = std::filesystem::path(pendingDeleteRequest->path).filename().string();
            ImGui::TextWrapped("Delete %s?", fileName.c_str());
            ImGui::Separator();
            if (pendingDeleteRequest->isDirectory) {
                ImGui::TextWrapped("Warning: this will permanently delete the folder and everything inside it.");
            } else {
                ImGui::TextWrapped("Warning: this will permanently delete this source file.");
            }
            ImGui::TextWrapped("%s", MakeDisplayPath(pendingDeleteRequest->path).c_str());

            bool closePopup = false;
            if (ImGui::Button("Delete")) {
                std::error_code errorCode;
                bool deleted = false;
                if (pendingDeleteRequest->isDirectory) {
                    deleted = std::filesystem::remove_all(pendingDeleteRequest->path, errorCode) > 0;
                } else {
                    deleted = std::filesystem::remove(pendingDeleteRequest->path, errorCode);
                }

                if (deleted && !errorCode) {
                    Refresh();
                    result.refreshed = true;
                    result.removedPaths.push_back(pendingDeleteRequest->path);
                    result.statusText = "Deleted " + MakeDisplayPath(pendingDeleteRequest->path);
                    closePopup = true;
                } else {
                    result.statusText = "Failed to delete " + MakeDisplayPath(pendingDeleteRequest->path);
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                closePopup = true;
            }

            if (closePopup) {
                pendingDeleteRequest.reset();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

void CodeEditorProjectWindow::DrawContextMenuPopup(DrawResult& result) {
    if (!ImGui::BeginPopup("file-tree-context-menu")) {
        return;
    }

    if (!pendingContextMenuTarget.has_value()) {
        ImGui::EndPopup();
        return;
    }

    const bool isRoot = pendingContextMenuTarget->path == rootPath;
    const std::string targetDirectory = pendingContextMenuTarget->isFile
                                        ? CodeEditorPathUtils::NormalizePath(std::filesystem::path(pendingContextMenuTarget->path).parent_path().generic_string())
                                        : pendingContextMenuTarget->path;
    bool shouldCloseContextMenu = false;

    if (ImGui::MenuItem("New Source File")) {
        OpenCreatePopup(CreateKind::File, targetDirectory);
        shouldCloseContextMenu = true;
    }

    if (ImGui::MenuItem("New Folder")) {
        OpenCreatePopup(CreateKind::Folder, targetDirectory);
        shouldCloseContextMenu = true;
    }

    if (pendingContextMenuTarget->isFile) {
        if (ImGui::MenuItem("Open")) {
            result.openedPath = pendingContextMenuTarget->path;
            shouldCloseContextMenu = true;
        }
    }

    if (ImGui::MenuItem("Reveal In File System")) {
        PathHelper::RevealPath(pendingContextMenuTarget->path);
        shouldCloseContextMenu = true;
    }

    if (!isRoot) {
        const char* deleteLabel = pendingContextMenuTarget->isFile ? "Delete File..." : "Delete Folder...";
        if (ImGui::MenuItem(deleteLabel)) {
            OpenDeletePopup(pendingContextMenuTarget->path, !pendingContextMenuTarget->isFile);
            shouldCloseContextMenu = true;
        }
    }

    if (shouldCloseContextMenu) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

CodeEditorProjectWindow::DrawResult CodeEditorProjectWindow::Draw(const std::string& activePath) {
    DrawResult result;

    ImGui::Begin("Files");

    if (ImGui::Button("Refresh")) {
        Refresh();
        result.refreshed = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("New File")) {
        OpenCreatePopup(CreateKind::File, rootPath);
    }

    ImGui::SameLine();
    if (ImGui::Button("New Folder")) {
        OpenCreatePopup(CreateKind::Folder, rootPath);
    }

    ImGui::SameLine();
    ImGui::TextWrapped("%s", MakeDisplayPath(rootPath).c_str());

    if (shouldOpenContextMenu) {
        ImGui::OpenPopup("file-tree-context-menu");
        shouldOpenContextMenu = false;
    }

    if (shouldOpenCreateModal && pendingCreateRequest.has_value()) {
        ImGui::OpenPopup(pendingCreateRequest->kind == CreateKind::File ? CreateFilePopupId.data() : CreateFolderPopupId.data());
        shouldOpenCreateModal = false;
    }

    if (shouldOpenDeleteModal && pendingDeleteRequest.has_value()) {
        ImGui::OpenPopup(DeletePopupId.data());
        shouldOpenDeleteModal = false;
    }

    if (treeRoot.children.empty()) {
        ImGui::TextWrapped("No .cpp or .hpp files were found under the selected project root.");
    } else {
        DrawNode(treeRoot, activePath, result);
    }

    DrawContextMenuPopup(result);
    DrawCreatePopup(result);
    DrawDeletePopup(result);

    ImGui::End();
    return result;
}

void CodeEditorProjectWindow::DrawNode(const FileTreeNode& node, const std::string& activePath, DrawResult& result) {
    ImGui::PushID(node.fullPath.c_str());

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
    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        pendingContextMenuTarget = ContextMenuTarget{
                .path = node.fullPath,
                .isFile = node.isFile
        };
        shouldOpenContextMenu = true;
    }

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

    ImGui::PopID();
}
