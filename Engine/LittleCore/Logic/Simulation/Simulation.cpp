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

void SimulationBase::HandleEvent(void* event, InputHandler& inputHandler) {
    /*empty*/
}

void SimulationBase::Render(Renderer& renderer) {
    /*empty*/
}

void SimulationBase::Render(bgfx::ViewId viewId, const WorldTransform& cameraTransform, const Camera& camera,
                            Renderer* renderer) {
    /*empty*/
}

void SimulationBase::Reload() {
    /*empty*/
}

void SimulationBase::SetResources(RegistrySerializerBase& registrySerializer, DefaultResourceManager& resourceManager) {

}

