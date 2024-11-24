
#include <vector>
#include "ModuleFactory.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <entt/entt.hpp>

struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

struct Console : public IModule {

    std::string code;

    entt::registry registry;

    using entityList = std::vector<entt::entity>;

    entityList entities;

    void CreateEntity() {
        auto entity = registry.create();
        registry.emplace<Position>(entity) = {2,2};
        registry.emplace<Velocity>(entity) = {10,10};
        entities.push_back(entity);
    }

    void OnGui() override {

        ImGui::Begin("Entity Window");

            if (ImGui::Button("Create entity")) {
                CreateEntity();
            }

            entityList entitiesToDelete;
            int index = 0;
            for (auto e : entities) {
                auto &pos = registry.get<Position>(e);

                ImGui::PushID(index);
                if (ImGui::Button("Delete")) {
                    entitiesToDelete.push_back(e);
                }
                ImGui::PopID();

                ImGui::SameLine();
                ImGui::LabelText("Position", std::to_string(pos.x).c_str());

                ImGui::PushID(index);
                auto& vel = registry.get<Velocity>(e);

                ImGui::SameLine();
                ImGui::InputFloat("Velocity", &vel.x);

                registry.replace<Velocity>(e, vel);
                ImGui::PopID();
                index++;
            }

            for(auto e : entitiesToDelete) {
                entities.erase(std::find(entities.begin(), entities.end(), e));
                registry.destroy(e);
            }

            ImGui::End();

            DrawCodeEditor();

    }

    void Update(float dt) override {

        const auto& view = registry.view<Position, Velocity>();

        for(auto [entity, position, velocity] : view.each()) {
            position.x += velocity.x;
            position.y += velocity.y;
        }
    }

    void Render() override {

    }

    void DrawCodeEditor() {
        ImGui::Begin("Code editor");

        auto oldCode = code;
        ImGui::InputTextMultiline("Code", &code);
        if (code!=oldCode){
            SaveToTextFile();
        }

        if (ImGui::Button("Save")) {
            SaveToTextFile();
        }

        ImGui::End();
    }

    void SaveToTextFile() {
        std::ofstream file;
        file.open("test.txt");
        file << code;
    }
};

CreateModule(Console);