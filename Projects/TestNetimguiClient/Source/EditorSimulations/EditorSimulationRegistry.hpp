//
// Created by Jeppe Nielsen on 04/10/2025.
//


#pragma once
#include "Simulation.hpp"
#include "EditorSimulationContext.hpp"
#include "EditorSimulation.hpp"
#include "Renderer.hpp"
#include <vector>
#include <memory>

namespace LittleCore {
    class EditorSimulationRegistry {
    public:
        EditorSimulationRegistry(EditorSimulationContext& contett);
        ~EditorSimulationRegistry();

        bool TryGetFirst(EditorSimulation** editorSimulation);

    private:

        using Simulations = std::vector<std::unique_ptr<EditorSimulation>>;
        Simulations simulations;

        void AddSimulation(SimulationBase& simulation);
        void RemoveSimulation(SimulationBase& simulation);

        EditorSimulationContext& context;
    };
}
