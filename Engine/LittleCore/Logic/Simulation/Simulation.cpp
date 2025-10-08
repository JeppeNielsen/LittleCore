#include "Simulation.hpp"

using namespace LittleCore;

SimulationBase::SimulationCreatedFunction SimulationBase::SimulationCreated = nullptr;
SimulationBase::SimulationCreatedFunction SimulationBase::SimulationDestroyed = nullptr;

SimulationBase::SimulationBase() {
    if (SimulationCreated) {
        SimulationCreated(*this);
    }
}

SimulationBase::~SimulationBase() {
    if (SimulationDestroyed) {
        SimulationDestroyed(*this);
    }
}

void SimulationBase::Render(bgfx::ViewId viewId, const WorldTransform& cameraTransform, const Camera& camera,
                            Renderer* renderer) {
    /*empty*/
}
