
#include <vector>
#include "ModuleFactory.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <entt/entt.hpp>

struct Hierarchy : public IModule {

    EngineContext* context;

    void Initialize(EngineContext& context) override {
        this->context = &context;
    }

    void OnGui() override {

        ImGui::Begin("Hierarchy");

        if (context->registryCollection->registries.empty()) {
            ImGui::Text("No registries detected");
        } else {

            auto &registry = *context->registryCollection->registries[0];
            auto view = registry.view<entt::entity>();
            int count = 0;
            for(auto e : view) {
                count++;
            }
            std::string o = "Number of entities: " + std::to_string(count);
            ImGui::Text(o.c_str());
        }

        ImGui::End();

    }

    void Update(float dt) override {
    }

    void Render() override {

    }

};

CreateModule(Hierarchy);