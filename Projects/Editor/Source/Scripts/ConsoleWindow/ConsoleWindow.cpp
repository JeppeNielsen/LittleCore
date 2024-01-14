
#include <iostream>
#include <vector>
#include "IModuleState.hpp"
#include "ModuleStateFactory.hpp"


struct ConsoleWindow : public IModuleState {

    ConsoleWindow(float startTime) : time(startTime) {

    }

    float time = 0;

    void Update(float dt) override {
        time+=dt*100;
    }

    void Render() override {
        std::cout << "Time : "<<time<<"\n";
    }

};

extern "C" {
    IModuleState* CreateModuleState(ModuleStateFactory* factory) {
        return factory->CreateState<ConsoleWindow>(-100);
    }

    void DeleteModuleState(IModuleState* state) {
        delete state;
    }
}