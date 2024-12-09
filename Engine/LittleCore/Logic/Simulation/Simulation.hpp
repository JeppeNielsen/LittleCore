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
    using UpdateSystems = SimulationSystemList<System...>;

    template <typename ...System>
    using RenderSystems = SimulationSystemList<System...>;

    template<typename UpdateSystems, typename RenderSystems>
    class Simulation {
    public:

        Simulation(entt::registry& registry)
                : registry(registry),
                  updateSystems(registry) {}


        void Update() {
           TupleHelper::for_each(updateSystems.systems, [] (auto& updateSystem) {
               updateSystem.Update();
           });
        }

        void Render(Renderer& renderer) {
            /*TupleHelper::for_each(renderSystems.systems, [&renderer] (auto& updateSystem) {
                updateSystem.Render(renderer);
            });
             */
        }

    private:
        entt::registry& registry;
        UpdateSystems updateSystems;
    };

}