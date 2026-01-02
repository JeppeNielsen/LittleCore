//
// Created by Jeppe Nielsen on 04/10/2025.
//

#include "EditorSimulationRegistry.hpp"
#include "Simulation.hpp"

using namespace LittleCore;

EditorSimulationRegistry::EditorSimulationRegistry(EditorSimulationContext& context) : context(context) {
    /*SimulationBase::SimulationCreated = [this](SimulationBase& simulation) {
        AddSimulation(simulation);
    };
    SimulationBase::SimulationDestroyed = [this](SimulationBase& simulation) {
        RemoveSimulation(simulation);
    };
     */
}

EditorSimulationRegistry::~EditorSimulationRegistry() {
    /*SimulationBase::SimulationCreated = nullptr;
    SimulationBase::SimulationDestroyed = nullptr;
     */
}

void EditorSimulationRegistry::AddSimulation(SimulationBase& simulation) {
    simulations.push_back(std::make_unique<EditorSimulation>(context, simulation));
}

void EditorSimulationRegistry::RemoveSimulation(LittleCore::SimulationBase& simulation) {
    simulations.erase(
            std::remove_if(
                    simulations.begin(),
                    simulations.end(),
                    [&simulation](const std::unique_ptr<EditorSimulation>& sim) {
                        return &sim.get()->simulation == &simulation;
                    }),
            simulations.end());
}

bool EditorSimulationRegistry::TryGetFirst(EditorSimulation** editorSimulation) {
    for(auto& simulation : simulations) {
        *editorSimulation = simulation.get();
        return true;
    }

    return false;
}

void EditorSimulationRegistry::Reload() {
    for(auto& simulation : simulations) {
        simulation->simulation.Reload();
    }
}
