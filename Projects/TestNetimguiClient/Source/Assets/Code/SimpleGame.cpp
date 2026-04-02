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

    void Reload() {
        std::string path = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/Assets/Pong/Assets/Bat/Stage.prefab";
        auto data = FileHelper::ReadAllText(path);
        simulation.registry.clear();
        auto error = Load(simulation.registry, data);
    }

    void OnInitialize() override {
        SerializedTypes<Types>();
        AddSimulation(simulation);
        
        Reload();
    }

    void OnUpdate(float dt) override {
        simulation.Update(dt);
    }

    void OnRender() override {

    }

    void OnGui() override {
        
        ImGui::Begin("File");

        if (ImGui::Button("Load")) {
            Reload();
        }

        ImGui::End();

    }

};



