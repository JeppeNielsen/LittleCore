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
    struct SystemsList {
        std::tuple<T...> systems;

        template<typename O>
        static constexpr std::tuple<O> CreateTuple(entt::registry& registry) {
            return std::make_tuple(std::ref(registry));
        }

        template<typename ...O>
        static constexpr std::tuple<O...> CreateTuples(entt::registry& registry) {
            return std::tuple_cat(std::make_tuple(std::ref(registry)));
        }

        static constexpr SystemsList<T...> Create(entt::registry& registry) {
            return {CreateTuples<T...>(registry)};
        }

    };

    template <typename ...System>
    using UpdateSystems = SystemsList<System...>;

    template <typename ...System>
    using RenderSystems = SystemsList<System...>;

    template<typename UpdateSystems, typename RenderSystems>
    class Simulation {
    public:

        Simulation(entt::registry& registry)
                : registry(registry),
                  updateSystems(UpdateSystems::Create(registry)),
                  renderSystems(RenderSystems::Create(registry)) {}


        void Update() {
           TupleHelper::for_each(updateSystems.systems, [] (auto& updateSystem) {
               updateSystem.Update();
           });
        }

        void Render(Renderer& renderer) {
            TupleHelper::for_each(renderSystems.systems, [&renderer] (auto& updateSystem) {
                updateSystem.Render(renderer);
            });
        }

    private:
        entt::registry& registry;
        UpdateSystems updateSystems;
        RenderSystems renderSystems;
    };

}