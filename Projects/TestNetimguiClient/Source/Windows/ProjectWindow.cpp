//
// Created by Jeppe Nielsen on 13/10/2025.
//

#include "ProjectWindow.hpp"



#include <imgui.h>
#include "ProjectWindow.hpp"
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include "PathHelper.hpp"
#include <unordered_map>

using namespace LittleCore;

static std::string draggingResourcePath;

struct Node {
    std::string name;
    std::string path;
    std::string fullPath;
    std::map<std::string, Node> children;
    bool isFile = false; // Mark if this node represents a file
};

// Helper function to split a file path into components
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

// Build the hierarchy tree from the file paths
void buildTree(Node& root, const std::unordered_map<std::string, std::string>& filePaths, const std::size_t cutoffPath) {
    for (const auto& pathKeyValue : filePaths) {
        std::string fullPath = pathKeyValue.first;
        std::string path = pathKeyValue.first.substr(cutoffPath);
        auto components = splitPath(path);
        Node* current = &root;
        for (size_t i = 0; i < components.size(); ++i) {
            const auto& part = components[i];
            if (i == components.size() - 1) {
                // Mark the last component as a file

                auto& file = current->children[part];
                file.name = part;
                file.path = path;
                file.fullPath = fullPath;
            } else {
                // Navigate or create the directory node
                current = &current->children[part];
                current->name = part;
                current->path = path;
                current->fullPath = fullPath;
            }
        }
    }
}

// Recursive function to traverse and print the hierarchy
void traverseTree(const Node& node, const std::string& prefix = "") {


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

void ProjectWindow::Draw(Project& project) {
    ImGui::Begin("Project");

    if (ImGui::Button("Refresh")) {
        project.resourcePathMapper.RefreshFromRootPath(project.rootPath);
    }


    Node root;
    root.name = "Project";
    buildTree(root, project.resourcePathMapper.PathToGuids(), project.rootPath.size());
    traverseTree(root);

    ImGui::End();
}
