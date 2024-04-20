
#include <iostream>
#include <vector>
#include "ModuleStateFactory.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"

struct ConsoleState : public IModuleState {

    float sliderValue = 50.0f;
    std::string name;

    float time = 0;

    void Initialize(EngineContext& context) override {

    }

    void OnGui() override {

        ImGui::Begin("ConsoleWindow");

        ImGui::SliderFloat("Value", &sliderValue, 0, 100);

        ImGui::InputText("Name", &name);

        ImGui::End();
    }

    void Update(float dt) override {
        time+=dt*1000;
    }

    void Render() override {

    }

};

extern "C" {
    IModuleState* CreateModuleState(ModuleStateFactory* factory) {
        return factory->CreateState<ConsoleState>();
    }

    void DeleteModuleState(IModuleState* state) {
        delete state;
    }
}