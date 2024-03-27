
#include <iostream>
#include <vector>
#include "ModuleStateFactory.hpp"

struct ConsoleState : public IModuleState {

    float time = 0;

    void Initialize(EngineContext& context) override {
        auto window = context.CreateWindow();

        std::cout << "Console Window : "<< window << std::endl;

    }

    void Update(float dt) override {
        time+=dt*1000;
    }

    void Render() override {
        std::cout << "Time from console: "<<time<<"\n";
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