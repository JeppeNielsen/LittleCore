//
// Created by Jeppe Nielsen on 13/04/2024.
//
#include "Engine.hpp"
#include "ImGuiController.hpp"
#include "imgui.h"
#include <vector>

using namespace LittleCore;


// Example data structure for nodes
struct TreeNode {
    std::string name;
    std::vector<TreeNode*> children;
    TreeNode* parent;
};

static TreeNode* root = nullptr;

void ShowDragAndDropTree(TreeNode* node, TreeNode* parent = nullptr) {
    if (ImGui::TreeNode(node->name.c_str())) {

        // Drag source
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None | ImGuiDragDropFlags_AcceptBeforeDelivery)) {
            node->parent = parent;
            ImGui::SetDragDropPayload("DND_TREE_NODE", &node, sizeof(TreeNode*)); // Set payload
            ImGui::Text("Dragging: %s", node->name.c_str());
            ImGui::EndDragDropSource();
        }

        // Drop target
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TREE_NODE")) {
                TreeNode* draggedNode = *(TreeNode**)payload->Data;
                if (draggedNode != node) {
                    if (draggedNode->parent!= nullptr) {
                        auto it = std::find(draggedNode->parent->children.begin(), draggedNode->parent->children.end(),draggedNode);
                        draggedNode->parent->children.erase(it);
                    }
                    node->children.push_back(draggedNode); // Add the dragged node to this node's children
                }
            }
            ImGui::EndDragDropTarget();
        }

        // Recursively draw child nodes
        for (auto child : node->children) {
            ShowDragAndDropTree(child, node);
        }

        ImGui::TreePop();
    }
}

void ShowTreeWithDragAndDrop() {
    if (root == nullptr) {

        root = new TreeNode();
        root->name = "Root";

        auto child1 = new TreeNode();
        child1->name = "Child1";

        auto child2 = new TreeNode();
        child2->name = "Child2";

        auto subChild1 = new TreeNode();
        subChild1->name = "SubChild1";

        auto subChild2 = new TreeNode();
        subChild2->name = "SubChild2";

        root->children.push_back(child1);
        root->children.push_back(child2);

        child1->children.push_back(subChild1);
        child2->children.push_back(subChild2);




    }

    if (ImGui::Begin("Drag and Drop Tree View")) {
        ShowDragAndDropTree(root);
    }
    ImGui::End();
}


struct Color {
    float r,g,b,a;

    uint32_t GetUint() {
        uint8_t red = (255)*r;
        uint8_t green = (255)*g;
        uint8_t blue = (255)*b;
        uint8_t alpha = (255)*a;

        return red << 24 | green << 16 | blue << 8 | alpha;
    }

};
struct ImguiTest : IState {

    ImGuiController gui;

    Color backgroundColor;

    bool showWindow = false;
    
    int value;

    void Initialize() override {
        
        gui.Initialize(mainWindow, [this]() {
            OnGUI();
        });

        gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/TestImGui/Source/Fonts/LucidaG.ttf", 12);
   }

    void HandleEvent(void* event) override {
        gui.HandleEvent(event);
    }

    void OnGUI() {

        ImGui::DockSpaceOverViewport();

        ImGui::Begin("My test window");

        if (ImGui::Button("Show extra window")) {
            showWindow = !showWindow;
        }

        ImGui::End();

        ImGui::Begin("Window 2");

        ImGui::ColorEdit4("Color", &backgroundColor.r);

        ImGui::End();
        
        ImGui::Begin("Window 3");

        ImGui::ColorEdit4("Color", &backgroundColor.r);

        ImGui::End();
		
        if (showWindow) {
            ImGui::Begin("Extra window");
            
            ImGui::DragInt("Test int", &value);

            ImGui::End();
        }

        ImGui::ShowDemoWindow();

        ShowTreeWithDragAndDrop();
    }

    void Update(float dt) override {

    }

    void Render() override {
        gui.Render();
    }

};

int main() {
    Engine e({"Test Netimgui"});
    e.Start<ImguiTest>();
    return 0;
}

