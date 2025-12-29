//
// Created by Jeppe Nielsen on 28/12/2025.
//


#pragma once
#include <memory>
#include "IState.hpp"
#include "Simulation.hpp"
#include "GuiResourceDrawers.hpp"

namespace LittleCore {
    class MainState : public IState {
    public:
        MainState();
        virtual ~MainState();
    private:
        void Initialize() override;
        void Update(float dt) override;
        void Render() override;
        void HandleEvent(void* event) override;
    private:
        struct Parameters;
        Parameters* parameters;
    protected:
        void AddSimulation(SimulationBase& simulation);
        virtual void OnGui();
        virtual void OnInitialize() = 0;
        virtual void OnUpdate(float dt) = 0;
        virtual void OnRender() = 0;
    };
}
