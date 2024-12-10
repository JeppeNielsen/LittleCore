//
// Created by Jeppe Nielsen on 04/12/2024.
//

#pragma once

#include <Renderer.hpp>
#include "TupleHelper.hpp"
#include <entt/entt.hpp>
#include <tuple>

namespace LittleCore {

    template<typename ...T>
    struct SimulationSystemList {

        SimulationSystemList(entt::registry& registry) : systems(std::tuple<T...>(GetRegistry<T>(registry)...)) {}

        template<typename O>
        constexpr entt::registry& GetRegistry(entt::registry& registry) {
            return std::forward<entt::registry&>(registry);
        }

        std::tuple<T...> systems;
    };

    template <typename ...System>
    using InputSystems = SimulationSystemList<System...>;

    template <typename ...System>
    using UpdateSystems = SimulationSystemList<System...>;

    template <typename ...System>
    using RenderSystems = SimulationSystemList<System...>;

    template<typename InputSystems, typename UpdateSystems, typename RenderSystems>
    class Simulation {
    public:

        Simulation(entt::registry& registry)
                : registry(registry),
                  inputSystems(registry),
                  updateSystems(registry),
                  renderSystems(registry) {}

        void HandleEvent(void* event) {
            TupleHelper::for_each(inputSystems.systems, [=] (auto& inputSystem) {
                inputSystem.HandleEvent(event);
            });
        }

        void Update() {
            TupleHelper::for_each(inputSystems.systems, [] (auto& inputSystem) {
                inputSystem.Update();
            });
            TupleHelper::for_each(updateSystems.systems, [] (auto& updateSystem) {
               updateSystem.Update();
            });
            TupleHelper::for_each(renderSystems.systems, [] (auto& renderSystem) {
                renderSystem.Update();
            });
        }

        void Render(Renderer& renderer) {
            TupleHelper::for_each(renderSystems.systems, [&renderer] (auto& renderSystem) {
                renderSystem.Render(&renderer);
            });
        }

        void Render(bgfx::ViewId viewId, const WorldTransform& cameraTransform, const Camera& camera, Renderer* renderer) {
            TupleHelper::for_each(renderSystems.systems, [&] (auto& renderSystem) {
                renderSystem.Render(viewId, cameraTransform, camera, renderer);
            });
        }

    private:
        entt::registry& registry;
        InputSystems inputSystems;
        UpdateSystems updateSystems;
        RenderSystems renderSystems;
    };

}