
#include <iostream>
#include <vector>
#include "ModuleStateFactory.hpp"
#include "Vector2.hpp"

struct HierarchyState : public IModuleState {

    float time = 0;

    void Initialize(EngineContext& context) override {
        auto window = context.CreateWindow();

        std::cout << "Hierarchy Window : "<< window << std::endl;

        std::vector<float> numbers = {1,2,3,4,5,6,7,8,9};

        for(auto n : numbers) {
            //time += n*0.1;
        }

        Vector2 vec {3,4};
        time += vec.Length();

    }

    void Update(float dt) override {
        time+=dt*100;
    }

    void Render() override {
        std::cout << "Time from hierarchy: "<<time<<"\n";
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