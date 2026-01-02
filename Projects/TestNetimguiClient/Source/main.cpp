//
// Created by Jeppe Nielsen on 27/12/2025.
//

#include "Engine.hpp"
#include "MainState.hpp"
#include <imgui.h>
#include "DefaultSimulation.hpp"
#include "FileHelper.hpp"

using namespace LittleCore;

struct Velocity {
    glm::vec3 speed;
};

struct Rotater {
    float speed;
};

struct MoverSystem : LittleCore::SystemBase {

    void Update(float dt) {
        for(auto[entity, velocity, transform]  : registry.view<const Velocity, LocalTransform>().each()) {
            transform.position += velocity.speed * dt;
            registry.patch<LocalTransform>(entity);
        }
    }
};

struct RotatorSystem : LittleCore::SystemBase {

    void Update(float dt) {
        for(auto[entity, rotator, transform]  : registry.view<const Rotater, LocalTransform>().each()) {
            transform.rotation *= quat({0,rotator.speed * dt ,0 });
            registry.patch<LocalTransform>(entity);
        }
    }

};



struct SimpleGame : public LittleCore::MainState {

    LittleCore::CustomSimulation<MoverSystem> simulation;

    virtual ~SimpleGame() {}

    void OnInitialize() override {
        std::cout << "Came here 1" << std::endl;
        SerializedTypes<Velocity, Rotater>();
        std::cout << "Came here 2" << std::endl;
        AddSimulation(simulation);
        std::cout << "Came here 3" << std::endl;
    }

    void OnUpdate(float dt) override {
        simulation.Update(dt);
    }

    void OnRender() override {

    }

    void OnGui() override {
        ImGui::Begin("File");

        if (ImGui::Button("Save")) {
            auto data = Save(simulation.registry);
            FileHelper::TryWriteAllText("Scene.json", data);
        }

        if (ImGui::Button("Load")) {
            auto data = FileHelper::ReadAllText("Scene.json");
            simulation.registry.clear();
            auto error = Load(simulation.registry, data);

        }

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