
//
// Created by Jeppe Nielsen on 27/12/2025.
//

#include "MainState.hpp"
#include <imgui.h>
#include "DefaultSimulation.hpp"
#include "FileHelper.hpp"
#include "Systems.hpp"
#include "Vector2.hpp"

using namespace LittleCore;

struct OnlyGame : public LittleCore::MainState {

    LittleCore::CustomSimulation<MoverSystem, BobberSystem, CollisionSystem, SizableSystem> simulation;
    
    virtual ~OnlyGame() {}

    void Reload() {
        std::string path = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/Assets/Pong/Assets/Bat/Stage.prefab";
        auto data = FileHelper::ReadAllText(path);
        simulation.registry.clear();
        auto error = Load(simulation.registry, data);
    }

    void OnInitialize() override {
        SerializedTypes<Types>();
        AddSimulation(simulation);
        AddSimulation(simulation);
        Reload();
        
        Vector2 myVector { 3,4};
        
        std::cout << "Vector length = " << myVector.Length() << std::endl;
        
        
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










