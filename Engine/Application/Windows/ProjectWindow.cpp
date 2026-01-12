//
// Created by Jeppe Nielsen on 13/10/2025.
//

#include "ProjectWindow.hpp"
#include <imgui.h>
#include <string>
#include "PathHelper.hpp"
#include "FileTreeFactory.hpp"

using namespace LittleCore;

static std::string draggingResourcePath;

// Recursive function to traverse and print the hierarchy
void traverseTree(const FileTreeNode& node, const std::string& prefix = "") {

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_None;

    if (node.children.empty()) {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }

    //if (currentState->selectedEntity == entity) {
    //    nodeFlags |= ImGuiTreeNodeFlags_Selected;
    //}

    nodeFlags |= ImGuiTreeNodeFlags_FramePadding;

    nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
    nodeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    bool isUnfolded = ImGui::TreeNodeEx(node.name.c_str(), nodeFlags);


    if (ImGui::BeginPopupContextItem(("ProjectRightClickMenu" + node.path).c_str())) {
        if (ImGui::MenuItem("Reveal in File System")) {
            PathHelper::RevealPath(node.fullPath);
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None | ImGuiDragDropFlags_AcceptBeforeDelivery)) {
        draggingResourcePath = node.fullPath;
        ImGui::SetDragDropPayload("ResourcePath", static_cast<const void*>(draggingResourcePath.data()), draggingResourcePath.size()); // Set payload
        ImGui::Text("Dragging: %s", draggingResourcePath.c_str());
        ImGui::EndDragDropSource();
    }

    if (isUnfolded) {
        for (const auto& [name, child] : node.children) {
            traverseTree(child);
        }
        ImGui::TreePop();
    }


}

void ProjectWindow::Draw(Project& project, DefaultResourceManager& resourceManager,
                         EditorSimulationRegistry& editorSimulationRegistry) {
    ImGui::Begin("Project");

    if (ImGui::Button("Refresh")) {
        project.resourcePathMapper.RefreshFromRootPath(project.rootPath);

        project.resourceHashMapper.IterateChangedPaths(project.resourcePathMapper, [&resourceManager](const auto& guid) {
            resourceManager.Reload(guid);
        });
        editorSimulationRegistry.Reload();
    }


    FileTreeNode root;
    FileTreeFactory fileTreeFactory;
    root.name = "Project";
    fileTreeFactory.CreateTree(root, project.resourcePathMapper.PathToGuids(), project.rootPath);
    traverseTree(root);

    ImGui::End();
}

