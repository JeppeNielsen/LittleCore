//
// Created by Jeppe Nielsen on 15/04/2024.
//

#include "Engine.hpp"
#include "MainState/MainState.hpp"

using namespace LittleCore;

int main() {
    Engine e;
    e.Start<MainState>();
    return 0;
}