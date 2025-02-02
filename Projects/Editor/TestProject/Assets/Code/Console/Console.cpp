
#include <vector>
#include "ModuleFactory.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <entt/entt.hpp>
#include "DefaultSimulation.hpp"
#include "Hierarchy.hpp"
#include "LocalTransform.hpp"
#include "WorldTransform.hpp"
#include "Camera.hpp"
#include "ComponentEditorCollection.hpp"
#include "ResourceLoader.hpp"
#include "ComponentEditor.hpp"
#include "GuiHelper.hpp"
#include <cmath>

using namespace LittleCore;

struct Rotatable {
    float speed;
};

struct RotatableSystem {

    entt::registry& registry;

    RotatableSystem(entt::registry& registry) : registry(registry) {}

    void Update() {
        for(auto e : registry.view<Rotatable, LocalTransform>()) {
            auto& rotatable = registry.get<Rotatable>(e);
            auto& localTransform = registry.get<LocalTransform>(e);

            vec3 euler = glm::eulerAngles(localTransform.rotation);
            euler.z += rotatable.speed;
            localTransform.rotation = glm::quat(euler);

            registry.patch<LocalTransform>(e);
        }
    }

};

class RotatableComponentEditor : public ComponentEditor<Rotatable> {
protected:
    bool Draw(entt::registry& registry, entt::entity entity, Rotatable& component) override {
        GuiHelper::DrawHeader("Rotatable");
        ImGui::InputFloat("Speed", &component.speed);
        return ImGui::IsItemEdited();
    }
};

struct Pulsator {
    float speed = 0.01f;
    float minScale = 1.0f;
    float maxScale = 1.5f;
    float time = 1;
};

struct PulsatorSystem {

    entt::registry& registry;

    PulsatorSystem(entt::registry& registry) : registry(registry) {}

    void Update() {
        for(auto e : registry.view<Pulsator, LocalTransform>()) {
            Pulsator& pulsator = registry.get<Pulsator>(e);
            LocalTransform& localTransform = registry.get<LocalTransform>(e);

            pulsator.time += pulsator.speed;

            float t = 0.5f + sinf(pulsator.time) * 0.5f;
            float scale = pulsator.minScale + (pulsator.maxScale - pulsator.minScale) * t;

            localTransform.scale = vec3(scale, scale, scale);

            registry.patch<LocalTransform>(e);
        }
    }

};

class PulsatorComponentEditor : public ComponentEditor<Pulsator> {
protected:
    bool Draw(entt::registry& registry, entt::entity entity, Pulsator& component) override {
        GuiHelper::DrawHeader("Pulsator");
        ImGui::InputFloat("Speed", &component.speed);
        ImGui::InputFloat("Time", &component.time);
        ImGui::InputFloat("Min Scale", &component.minScale);
        ImGui::InputFloat("Max Scale", &component.maxScale);

        return ImGui::IsItemEdited();
    }
};

struct World {
    std::shared_ptr<entt::registry> registry;

    World() : registry(std::make_shared<entt::registry>()), simulation(*registry.get()) {

    }

    LittleCore::CustomSimulation<RotatableSystem, PulsatorSystem> simulation;

};

entt::entity CreateQuad(entt::registry& registry, glm::vec3 position, entt::entity parent = entt::null) {

    auto quad = registry.create();
    registry.emplace<LocalTransform>(quad).position = position;
    registry.emplace<WorldTransform>(quad);
    registry.emplace<Hierarchy>(quad).parent = parent;
    auto& mesh = registry.emplace<Mesh>(quad);
    mesh.vertices.push_back({{-1,-1,0}, 0x00FF0000 , {0,0}});
    mesh.vertices.push_back({{1,-1,0}, 0xff000000 , {0,1} });
    mesh.vertices.push_back({{1,1,0}, 0xff000000, {1,1}});
    mesh.vertices.push_back({{-1,1,0}, 0xffFF0000,{1,0}});
    mesh.triangles.push_back(0);
    mesh.triangles.push_back(1);
    mesh.triangles.push_back(2);

    mesh.triangles.push_back(0);
    mesh.triangles.push_back(2);
    mesh.triangles.push_back(3);

    registry.emplace<Renderable>(quad);
    registry.emplace<LocalBoundingBox>(quad);
    registry.emplace<WorldBoundingBox>(quad);
    registry.emplace<Rotatable>(quad).speed = 0.1f;
    registry.emplace<Texturable>(quad);

    return quad;
}

struct Console : public IModule {

    std::vector<std::unique_ptr<World>> worlds;

    using entityList = std::vector<entt::entity>;

    entityList entities;

    EditorRenderer* editorRenderer;
    ResourceLoader* resourceLoader;

    bool hasRendered = false;

    ImTextureID textureId;
    ComponentEditorCollection<RotatableComponentEditor, PulsatorComponentEditor> componentEditorCollection;

    void CreateEntity() {

        auto& registry = *worlds[0]->registry.get();

        auto quad = CreateQuad(registry, {entities.size(),0,0});
        entities.push_back(quad);

        registry.get<Renderable>(quad).shader = resourceLoader->LoadShader("65886F92DEC94836A9E2FEA6C3483543");
        registry.get<Texturable>(quad).texture = resourceLoader->LoadTexture("4BC3DABE71914C1782E4333052810803");
    }

    void Initialize(EngineContext& context) override {
        worlds.push_back(std::make_unique<World>());
        context.registryManager->Add("Main", worlds[0]->registry);
        editorRenderer = context.editorRenderer;
        resourceLoader = context.resourceLoader;
        context.componentDrawer->Install([this](entt::registry& registry, entt::entity entity) {
            componentEditorCollection.Draw(registry, entity);
        });
        context.componentFactory->Install([this](entt::registry& registry, entt::entity entity, const std::string& componentId) {
            componentEditorCollection.CreateComponent(registry, entity, componentId);
        }, [this]() {
            return componentEditorCollection.GetComponentIds();
        });
    }

    void CreateCamera() {

        auto& registry = *worlds[0]->registry.get();

        auto cameraObject = registry.create();
        registry.emplace<LocalTransform>(cameraObject).position = {0, 0, -10};
        registry.emplace<WorldTransform>(cameraObject);
        registry.emplace<Hierarchy>(cameraObject);

        auto &camera = registry.emplace<Camera>(cameraObject);
        camera.fieldOfView = 60.0f;
        camera.near = 1;
        camera.far = 20;
        camera.viewRect = {{0,    0},
                           {1.0f, 1.0f}};

        entities.push_back(cameraObject);
    }

    void OnGui() override {

        ImGui::Begin("Entity Window");

            if (ImGui::Button("Clear registries")) {
                worlds.clear();
            }

            if (worlds.size()>0) {
                if (ImGui::Button("Create camera")) {
                    CreateCamera();
                }

                if (ImGui::Button("Create entity")) {
                    CreateEntity();
                }

                entityList entitiesToDelete;
                int index = 0;
                for (auto e: entities) {

                    ImGui::PushID(index);
                    if (ImGui::Button("Delete")) {
                        entitiesToDelete.push_back(e);
                    }

                    ImGui::PopID();

                    index++;
                }

                for (auto e: entitiesToDelete) {
                    entities.erase(std::find(entities.begin(), entities.end(), e));
                    worlds[0]->registry.get()->destroy(e);
                }
            }
        ImGui::End();

        ImGui::Begin("Game");

        ImGui::Image(textureId, ImVec2((float) 1024, (float) 1024));
        ImGui::End();
    }

    void Update(float dt) override {
        if (worlds.size()>0) {
            worlds[0]->simulation.Update();
        }
    }

    void Render(EditorRenderer* editorRenderer) override {


        editorRenderer->Render("Test", 1024,1024, [this](Renderer& renderer) {
            if (worlds.size() == 0) {
                return;
            }
            worlds[0]->simulation.Render(renderer);
        });
        hasRendered = true;

        textureId = editorRenderer->GetTexture("Test");

    }

};

CreateModule(Console);