//
// Created by Jeppe Nielsen on 20/03/2026.
//

#include "Engine.hpp"
#include "MainState/MainState.hpp"

using namespace LittleCore;

int main() {
    Engine engine({
        .mainWindowTitle = "EditorHub"
    });
    engine.Start<MainState>();
    return 0;
}
