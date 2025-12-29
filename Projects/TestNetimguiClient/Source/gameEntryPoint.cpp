//
// Created by Jeppe Nielsen on 27/12/2025.
//

#include "Engine.hpp"
#include "MainState.hpp"
#include <imgui.h>
#include "DefaultSimulation.hpp"

struct Mover {
    float speed;

};


struct MoverSystem : LittleCore::SystemBase {



    void Update(float dt) {
        for(auto[entity, mover]  : registry.view<Mover>().each()) {
            std::cout << std::format("Entity #{0} has mover with speed {1}", (int)entity, mover.speed)<<"\n";
        }
    }

};

struct SimpleGame : public LittleCore::MainState {

    LittleCore::CustomSimulation<MoverSystem> simulation;

    void OnInitialize() override {

        AddSimulation(simulation);

        auto entity = simulation.registry.create();
        simulation.registry.emplace<Mover>(entity);

        auto entity2 = simulation.registry.create();
        simulation.registry.emplace<Mover>(entity2).speed = 2.0f;
    }

    void OnUpdate(float dt) override {

        simulation.Update(dt);

    }

    void OnRender() override {



    }

    void OnGui() override {
        ImGui::Begin("Project");

        ImGui::Button("click me");

        ImGui::End();

    }

};

int main() {
    using namespace LittleCore;
    Engine engine({
        .mainWindowTitle = "Editor",
        .showWindow = true
    });
    engine.Start<SimpleGame>();
}