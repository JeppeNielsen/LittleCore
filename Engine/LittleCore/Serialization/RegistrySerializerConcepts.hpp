//
// Created by Jeppe Nielsen on 05/11/2025.
//

#pragma once
#include <entt/entt.hpp>

namespace LittleCore {

    template <typename T, typename TComponent, typename TSerializedComponent>
        concept HasSerializeWithRegistryAndEntity = requires(T t, TComponent component, TSerializedComponent serializedComponent, const entt::registry& registry, entt::entity entity) {
    {
        t.Serialize(component, serializedComponent, registry, entity) } -> std::same_as<void>;
    };

}