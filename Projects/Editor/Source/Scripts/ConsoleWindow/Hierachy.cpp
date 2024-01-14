
#include <iostream>
#include <vector>
#include "ModuleStateFactory.hpp"

struct HierarchyState : public IModuleState {

    float time = 0;

    void Initialize(EngineContext& context) override {
        auto window = context.CreateWindow();

        std::cout << "Hierarchy Window : "<< window << std::endl;


    }

    void Update(float dt) override {
        time+=dt*100;
    }

    void Render() override {
        std::cout << "Time : "<<time<<"\n";
    }

};

extern "C" {
    IModuleState* CreateModuleState(ModuleStateFactory* factory) {
        return factory->CreateState<HierarchyState>();
    }

    void DeleteModuleState(IModuleState* state) {
        delete state;
    }
}