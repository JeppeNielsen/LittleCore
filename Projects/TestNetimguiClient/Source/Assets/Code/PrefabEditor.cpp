//
// Created by Jeppe Nielsen on 27/12/2025.
//

#include "MainState.hpp"
#include <imgui.h>
#include "DefaultSimulation.hpp"
#include "FileHelper.hpp"
#include "PrefabWindow.hpp"
#include "PrefabContext.hpp"
#include "Types.hpp"
#include "Systems.hpp"

struct PrefabEditor : public MainState {
    PrefabContext prefabContext;
    LittleCore::CustomSimulation<ClickColorerSystem> simulation;
    PrefabWindow prefabWindow;

    PrefabEditor() : prefabWindow(GetProject(), prefabContext, [this](const FileTreeNode& node, PrefabWindow::PrefabEvent event) {
        PrefabWindowMenu(node, event);
    }) {

    }

    void OnInitialize() override {
        SerializedTypes<Types>();
        AddSimulation(simulation);
    }

    void OnUpdate(float dt) override {
        simulation.Update(dt);
    }

    void OnRender() override {

    }

    std::string GetPathFromNode(const FileTreeNode& node) {
        if (node.children.empty()) {
            return node.parent->fullPath;
        }
        return node.fullPath;
    }

    void PrefabWindowMenu(const FileTreeNode& node, PrefabWindow::PrefabEvent event) {

        switch (event) {
            case PrefabWindow::PrefabEvent::NewFile: {
                std::string newFileName = "NewPrefab.prefab";

                decltype(simulation) emptySimulation;

                auto content = Save(emptySimulation.registry);

                auto path = GetPathFromNode(node);

                std::string newFilePath = path + "/" + newFileName;

                std::cout << newFilePath<<"\n";

                FileHelper::TryWriteAllText(newFilePath, content);

                GetProject().resourcePathMapper.RefreshFromRootPath(GetProject().rootPath);
                break;
            }
        }

    }

    void LoadPrefab(const std::string& path) {
        auto data = FileHelper::ReadAllText(path);
        simulation.registry.clear();
        auto error = Load(simulation.registry, data);
    }

    void SavePrefab(const std::string& path) {
        auto data = Save(simulation.registry);
        FileHelper::TryWriteAllText(path, data);
    }

    enum class ConfirmResult { None, Yes, No };

    ConfirmResult ConfirmModal(const char* popup_name, const char* message)
    {
        ConfirmResult result = ConfirmResult::None;

        ImGui::SetNextWindowSize(ImVec2(360, 0), ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal(popup_name, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextWrapped("%s", message);
            ImGui::Spacing();

            if (ImGui::Button("No"))
            {
                result = ConfirmResult::No;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Yes"))
            {
                result = ConfirmResult::Yes;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        return result;
    }

    void OnGui() override {

        if (GetProject().hierarchyChangedLastFrame) {
            prefabContext.isDirty = true;
        }



        prefabWindow.DrawUI();

        if (prefabContext.selectedPath!=prefabContext.previousSelectedPath) {

            if (!prefabContext.isDirty) {
                prefabContext.previousSelectedPath = prefabContext.selectedPath;
                LoadPrefab(prefabContext.selectedPath);
            } else {
                ImGui::OpenPopup("Confirm Delete");
                auto r = ConfirmModal("Confirm Delete", "Prefab has changes, discard?");
                if (r == ConfirmResult::Yes) {
                    prefabContext.previousSelectedPath = prefabContext.selectedPath;
                    LoadPrefab(prefabContext.selectedPath);
                    prefabContext.isDirty = false;
                } else if (r == ConfirmResult::No) {
                    prefabContext.selectedPath = prefabContext.previousSelectedPath;
                }
            }

        }

        ImGui::Begin("File");

        ImGui::Checkbox("Auto save", &prefabContext.settings.autoSave);

        if (prefabContext.isDirty) {
            if (prefabContext.settings.autoSave) {
                SavePrefab(prefabContext.selectedPath);
                prefabContext.isDirty = false;
            } else if (ImGui::Button("Save")) {
                SavePrefab(prefabContext.selectedPath);
                prefabContext.isDirty = false;
            }

        }

        ImGui::End();

    }

};
