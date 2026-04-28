//
// Created by Jeppe Nielsen on 07/01/2026.
//

#include "PrefabWindow.hpp"
#include <imgui.h>
#include "FileTreeFactory.hpp"
#include "PathHelper.hpp"
#include "FileHelper.hpp"

using namespace LittleCore;

PrefabWindow::PrefabWindow(const Project& project, PrefabContext& context, PrefabWindow::Callback callback)
: project(project), context(context), callback(callback) {

}

void PrefabWindow::DrawNode(const FileTreeNode& node) {

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

    if (node.fullPath == context.selectedPath) {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    bool isUnfolded = ImGui::TreeNodeEx(node.name.c_str(), nodeFlags);


    if (ImGui::BeginPopupContextItem(("ProjectRightClickMenu" + node.path).c_str())) {
        if (ImGui::MenuItem("New")) {
            callback(node, PrefabWindow::PrefabEvent::NewFile);
        }

        if (ImGui::MenuItem("Reveal in File System")) {
            PathHelper::RevealPath(node.fullPath);
        }

        ImGui::EndPopup();
    }

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
        context.selectedPath = node.fullPath;
    }

    if (isUnfolded) {
        for (const auto& [name, child] : node.children) {
            DrawNode(child);
        }
        ImGui::TreePop();
    }
}

void PrefabWindow::DrawUI() {
    ImGui::Begin("Prefabs");


    FileTreeNode root;
    FileTreeFactory fileTreeFactory;
    root.name = "Project";
    fileTreeFactory.CreateTree(root, project.resourcePathMapper.PathToGuids(), project.rootPath, [](const std::string& path) {
        return FileHelper::HasExtension(path, "prefab");
    });
    DrawNode(root);
    
    
/*
    static std::string selectedGuid;

    if (ImGui::BeginListBox("Prefabs", ImVec2(-FLT_MIN, 8 * ImGui::GetTextLineHeightWithSpacing())))
    {
        for(auto[path, guid] : project.resourcePathMapper.PathToGuids()) {

            bool is_selected = (selectedGuid == guid);
            if (ImGui::Selectable(path.c_str(), is_selected)) {
                selectedGuid = guid;
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }


        ImGui::EndListBox();
    }
*/


    ImGui::End();
}


