
#include <iostream>
#include "Engine.hpp"
#include <bgfx/bgfx.h>

using namespace LittleCore;

struct MainState : IState {

    void Update(float dt) override {
        std::cout << dt << std::endl;
    }

    void Render() override {
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x6495ED, 1.f, 0);
        bgfx::touch(0);



    }

};

int main() {
    Engine e;
    e.Start<MainState>();
    return 0;
}