
#include <vector>
#include "ModuleFactory.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <entt/entt.hpp>

struct Hierarchy : public IModule {

    void OnGui() override {

        ImGui::Begin("Hierarchy");

        ImGui::End();

    }

    void Update(float dt) override {
    }

    void Render() override {

    }

};

CreateModule(Hierarchy);