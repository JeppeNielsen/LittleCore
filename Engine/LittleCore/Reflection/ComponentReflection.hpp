//
// Created by Jeppe Nielsen on 27/10/2025.
//

#pragma once
#include <glaze/glaze.hpp>
#include "Hierarchy.hpp"
#include "PrefabInstance.hpp"

template<>
struct glz::meta<LittleCore::Hierarchy> {
    using T = LittleCore::Hierarchy;
    static constexpr auto value = glz::object(
            "parent", &T::parent
    );
};

template<>
struct glz::meta<LittleCore::PrefabInstance> {
    using T = LittleCore::PrefabInstance;
    static constexpr auto value = glz::object(
            "Prefab", &T::Prefab
    );
};