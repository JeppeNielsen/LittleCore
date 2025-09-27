//
// Created by Jeppe Nielsen on 04/12/2024.
//

#pragma once

#include <Renderer.hpp>
#include "TupleHelper.hpp"
#include <entt/entt.hpp>
#include <tuple>
#include "InputHandler.hpp"
#include <iostream>
#include "TypeUtility.hpp"

template <typename T>
concept HasUpdateFunction = requires(T t) {
    { t.Update() } -> std::same_as<void>;
};

template <typename T>
concept HasUpdateFunctionWithDelta = requires(T t, float dt) {
    { t.Update(dt) } -> std::same_as<void>;
};

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

    template<typename...>
    struct ConcatSimulationSystemLists_impl;

    template<typename... s1, typename... s2>
    struct ConcatSimulationSystemLists_impl<SimulationSystemList<s1...>, SimulationSystemList<s2...>> {
        using type = SimulationSystemList<s1..., s2...>;
    };

    template<typename A1, typename A2>
    using ConcatSimulationSystemLists = typename ConcatSimulationSystemLists_impl<A1,A2>::type;


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

        void HandleEvent(void* event, InputHandler& inputHandler) {
            TupleHelper::for_each(inputSystems.systems, [=, &inputHandler] (auto& inputSystem) {
                inputSystem.HandleEvent(event, inputHandler);
            });
        }

        void Update(float dt) {
            TupleHelper::for_each(inputSystems.systems, [] (auto& inputSystem) {
                inputSystem.Update();
            });
            TupleHelper::for_each(updateSystems.systems, [dt] (auto& updateSystem) {
                using updateSystemType = std::remove_cvref_t<decltype(updateSystem)>;
                if constexpr (HasUpdateFunctionWithDelta<updateSystemType>) {
                    updateSystem.Update(dt);
                } else if constexpr (HasUpdateFunction<updateSystemType>){
                    updateSystem.Update();
                }
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