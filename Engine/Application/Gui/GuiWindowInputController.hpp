//
// Created by Jeppe Nielsen on 09/10/2025.
//


#pragma once
#include "Simulation.hpp"

namespace LittleCore {
    class GuiWindowInputController {
    public:
        void Begin();
        void RunforSimulation(SimulationBase& simulation);
    private:
        float gameViewMinX;
        float gameViewMinY;
        float width;
        float height;

        bool wasWindowFocused;
    };
}
