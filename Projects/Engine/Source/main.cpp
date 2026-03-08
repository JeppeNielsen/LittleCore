
#include <iostream>
#include "Engine.hpp"
#include "SokolDirect.hpp"

using namespace LittleCore;

struct MainState : IState {
    void Initialize() override {}

    void Update(float dt) override {
        std::cout << dt << std::endl;
    }

    void Render() override {
        // Rendering is driven by the engine render systems.
    }

    void HandleEvent(void* event) override {
        (void)event;
    }

};

int main() {
    Engine e({"Engine"});
    e.Start<MainState>();
    return 0;
}
