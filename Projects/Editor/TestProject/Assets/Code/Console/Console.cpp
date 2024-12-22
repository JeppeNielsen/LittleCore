
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
#include "LocalTransformEditor.hpp"

using namespace LittleCore;

struct World {
    entt::registry registry;

    World() : simulation(registry), editors(registry) {

    }

    LittleCore::DefaultSimulation simulation;
    ComponentEditorCollection<LocalTransformEditor> editors;


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

    return quad;
}

struct Console : public IModule {

    World world;

    using entityList = std::vector<entt::entity>;

    entityList entities;

    EditorRenderer* editorRenderer;

    bool hasRendered = false;

    ImTextureID textureId;

    void CreateEntity() {

        auto quad = CreateQuad(world.registry, {entities.size(),0,0});
        entities.push_back(quad);
    }

    void Initialize(EngineContext& context) override {
        context.registryCollection->registries.push_back(&world.registry);
        editorRenderer = context.editorRenderer;
    }

    void CreateCamera() {

        auto& registry = world.registry;

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
                world.registry.destroy(e);
            }
        ImGui::End();

        ImGui::Begin("Game");

        if (hasRendered) {
            ImGui::Image(textureId, ImVec2((float) 512, (float) 512));
        }
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