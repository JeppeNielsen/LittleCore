//
// Created by Jeppe Nielsen on 28/12/2025.
//


#pragma once
#include <memory>
#include "IState.hpp"
#include "Simulation.hpp"
#include "GuiResourceDrawers.hpp"
#include "DefaultEntityGuiDrawer.hpp"
#include "DefaultRegistrySerializer.hpp"

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
        void AddEntityGuiDrawer(EntityGuiDrawerBase* entityGuiDrawerBase);
        void AddRegistrySerializer(RegistrySerializerBase* registrySerializerBase);
    private:
        struct Parameters;
        Parameters* parameters;
    protected:
        void AddSimulation(SimulationBase& simulation);
        virtual void OnGui();
        virtual void OnInitialize() = 0;
        virtual void OnUpdate(float dt) = 0;
        virtual void OnRender() = 0;
        std::string Save(const entt::registry& registry) const;
        std::string Load(entt::registry& registry, const std::string& data) const;

        template<typename ...T>
        void SerializedTypes() {
            DefaultEntityGuiDrawer<T...>* entityGuiDrawer = new DefaultEntityGuiDrawer<T...>();
            AddEntityGuiDrawer(entityGuiDrawer);
            DefaultRegistrySerializer<T...>* registrySerializer = new DefaultRegistrySerializer<T...>();
            AddRegistrySerializer(registrySerializer);
        }

    };
}
