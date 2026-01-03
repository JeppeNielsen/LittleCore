//
// Created by Jeppe Nielsen on 27/12/2025.
//

#include "Engine.hpp"
#include "Assets/Code/SimpleGame.cpp"

int main() {
    using namespace LittleCore;
    Engine engine({
        .mainWindowTitle = "Editor",
        .showWindow = true
    });
    engine.Start<SimpleGame>([](SimpleGame& state) {
        state.context.projectRoot = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/Assets";
        state.context.engineRoot = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimgui/Assets/";
        state.context.name = "SimpleGame";
    });
}
