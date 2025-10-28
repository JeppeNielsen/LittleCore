//
// Created by Jeppe Nielsen on 27/10/2025.
//

#pragma once
#include <glaze/glaze.hpp>
#include "Hierarchy.hpp"
#include "Prefab.hpp"

template<>
struct glz::meta<LittleCore::Hierarchy> {
    using T = LittleCore::Hierarchy;
    static constexpr auto value = glz::object(
            "parent", &T::parent
    );
};

template<>
struct glz::meta<LittleCore::Prefab> {
    using T = LittleCore::Prefab;
    static constexpr auto value = glz::object(
            "resource", &T::resource
    );
};