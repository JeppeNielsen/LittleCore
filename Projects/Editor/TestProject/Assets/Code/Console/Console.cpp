
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
#include "DefaultComponentEditors.hpp"
#include "ResourceLoader.hpp"

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
        ImGui::InputFloat("Speed", &component.speed);
        return ImGui::IsItemEdited();
    }
};


struct World {
    std::shared_ptr<entt::registry> registry;

    World() : registry(std::make_shared<entt::registry>()), simulation(*registry.get()), editors(*registry.get()) {

    }

    LittleCore::CustomSimulation<RotatableSystem> simulation;
    CustomComponentEditors<RotatableComponentEditor> editors;


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

    return quad;
}

struct Console : public IModule {

    World world;

    using entityList = std::vector<entt::entity>;

    entityList entities;

    EditorRenderer* editorRenderer;
    ResourceLoader* resourceLoader;

    bool hasRendered = false;

    ImTextureID textureId;

    void CreateEntity() {

        auto& registry = *world.registry.get();

        auto quad = CreateQuad(registry, {entities.size(),0,0});
        entities.push_back(quad);

        registry.get<Renderable>(quad).shader = resourceLoader->LoadShader("65886F92DEC94836A9E2FEA6C3483543");
    }

    void Initialize(EngineContext& context) override {
        context.registryManager->Add("Main", world.registry);
        editorRenderer = context.editorRenderer;
        resourceLoader = context.resourceLoader;
    }

    void CreateCamera() {

        auto& registry = *world.registry.get();

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

            if (ImGui::Button("Create camera")) {
                CreateCamera();
            }

            if (ImGui::Button("Create entity")) {
                CreateEntity();
            }

            entityList entitiesToDelete;
            int index = 0;
            for (auto e : entities) {

                ImGui::PushID(index);
                if (ImGui::Button("Delete")) {
                    entitiesToDelete.push_back(e);
                }


                /*ImGui::SameLine();

                ImGui::LabelText("Entity #", std::to_string(index).c_str());

                auto pos = world.registry.get<LocalTransform>(e).position;

                ImGui::SameLine();
                ImGui::LabelText("Position", std::to_string(pos.x).c_str());
                 */
                world.editors.Draw(e);

                ImGui::PopID();

                index++;
            }

            for(auto e : entitiesToDelete) {
                entities.erase(std::find(entities.begin(), entities.end(), e));
                world.registry.get()->destroy(e);
            }
        ImGui::End();

        ImGui::Begin("Game");

        ImGui::Image(textureId, ImVec2((float) 1024, (float) 1024));
        ImGui::End();
    }

    void Update(float dt) override {
        world.simulation.Update();
    }

    void Render(EditorRenderer* editorRenderer) override {


        editorRenderer->Render("Test", 1024,1024, [this](Renderer& renderer) {
            world.simulation.Render(renderer);
        });
        hasRendered = true;

        textureId = editorRenderer->GetTexture("Test");

    }

};

CreateModule(Console);