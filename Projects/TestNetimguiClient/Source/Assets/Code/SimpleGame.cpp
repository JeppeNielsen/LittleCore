//
// Created by Jeppe Nielsen on 27/12/2025.
//

#include "MainState.hpp"
#include <imgui.h>
#include "DefaultSimulation.hpp"
#include "FileHelper.hpp"
#include "Systems.hpp"

using namespace LittleCore;

struct SimpleGame : public LittleCore::MainState {

    LittleCore::CustomSimulation<MoverSystem, BobberSystem, CollisionSystem> simulation;

    virtual ~SimpleGame() {}

    void OnInitialize() override {
        SerializedTypes<Types>();
        AddSimulation(simulation);
    }

    void OnUpdate(float dt) override {
        simulation.Update(dt);
    }

    void OnRender() override {

    }

    void OnGui() override {
        ImGui::Begin("File");

        std::string path = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/Assets/Pong/Assets/Bat/Stage.prefab";

        if (ImGui::Button("Save")) {
            auto data = Save(simulation.registry);
            FileHelper::TryWriteAllText("Scene.json", data);
        }

        if (ImGui::Button("Load")) {
            auto data = FileHelper::ReadAllText(path);
            simulation.registry.clear();
            auto error = Load(simulation.registry, data);

        }

        ImGui::End();

    }

};
