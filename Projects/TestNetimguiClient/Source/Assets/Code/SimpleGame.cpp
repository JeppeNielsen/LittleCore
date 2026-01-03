//
// Created by Jeppe Nielsen on 27/12/2025.
//

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

struct Bobber {
    float speed = 1;
    float amplitude = 1;
    float progress = 0;
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

struct BobberSystem : LittleCore::SystemBase {

    void Update(float dt) {
        for(auto[entity, bobber, transform]  : registry.view<Bobber, LocalTransform>().each()) {
            bobber.progress += bobber.speed * dt;
            transform.position.x = sin(bobber.progress) * bobber.amplitude;
            registry.patch<LocalTransform>(entity);
        }
    }

};




struct SimpleGame : public LittleCore::MainState {

    LittleCore::CustomSimulation<MoverSystem, BobberSystem> simulation;

    virtual ~SimpleGame() {}

    void OnInitialize() override {
        SerializedTypes<Velocity, Rotater, Bobber>();
        AddSimulation(simulation);
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
