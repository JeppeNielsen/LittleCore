//
// Created by Jeppe Nielsen on 08/08/2024.
//

#include "Engine.hpp"
#include "TestRendering.hpp"

using namespace LittleCore;

int main() {
    Engine engine({"Test Rendering"});
    engine.Start<TestRendering>();
}