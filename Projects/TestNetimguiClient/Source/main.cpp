//
// Created by Jeppe Nielsen on 27/12/2025.
//

#include "Engine.hpp"
#include "Assets/Code/PrefabEditor.cpp"

int main() {
    using namespace LittleCore;
    Engine engine({
        .mainWindowTitle = "Prefab editor",
        .showWindow = true
    });
    engine.Start<PrefabEditor>([](PrefabEditor& state) {
        state.context.projectRoot = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/Assets";
        state.context.engineRoot = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimgui/Assets/";
        state.context.name = "PrefabEditor";
    });
}
