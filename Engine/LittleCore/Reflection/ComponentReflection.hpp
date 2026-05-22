//
// Created by Jeppe Nielsen on 27/10/2025.
//

#pragma once
#include <glaze/glaze.hpp>
#include "Hierarchy.hpp"
#include "Layouter.hpp"
#include "LayoutElement.hpp"
#include "Prefab.hpp"
#include "Renderable.hpp"
#include "StackLayout.hpp"

template<>
struct glz::meta<LittleCore::Hierarchy> {
    using T = LittleCore::Hierarchy;

    static constexpr auto read_parent = [](T& self, const entt::entity& parent) {
        self.parent = parent;
        self.previousParent = parent;
    };

    static constexpr auto write_parent = [](const T& self) {
        return self.parent;
    };

    static constexpr auto value = glz::object(
            "parent", glz::custom<read_parent, write_parent>,
            "children", &T::children
    );
};

template<>
struct glz::meta<LittleCore::Renderable> {
    using T = LittleCore::Renderable;
    static constexpr auto value = glz::object(
            "shader", &T::shader,
            "blendMode", &T::blendMode
    );
};

template<>
struct glz::meta<LittleCore::Layouter> {
    using T = LittleCore::Layouter;
    static constexpr auto value = glz::object(
            "anchorMin", &T::anchorMin,
            "anchorMax", &T::anchorMax,
            "anchoredPosition", &T::anchoredPosition,
            "sizeDelta", &T::sizeDelta,
            "pivot", &T::pivot
    );
};

template<>
struct glz::meta<LittleCore::StackLayout> {
    using T = LittleCore::StackLayout;
    static constexpr auto value = glz::object(
            "direction", &T::direction,
            "crossAlign", &T::crossAlign,
            "paddingMin", &T::paddingMin,
            "paddingMax", &T::paddingMax,
            "spacing", &T::spacing
    );
};

template<>
struct glz::meta<LittleCore::LayoutElement> {
    using T = LittleCore::LayoutElement;
    static constexpr auto value = glz::object(
            "min", &T::min,
            "preferred", &T::preferred,
            "max", &T::max,
            "grow", &T::grow,
            "shrink", &T::shrink,
            "ignoreLayout", &T::ignoreLayout
    );
};
