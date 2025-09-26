//
// Created by Jeppe Nielsen on 18/09/2025.
//
#include <glm/glm.hpp>
#include <glaze/glaze.hpp>
#include "BoundingBox.hpp"
#include "ResourceHandle.hpp"

template<>
struct glz::meta<glm::vec3> {
    using T = glm::vec3;
    static constexpr auto value = glz::object(
            "x", &T::x,
            "y", &T::y,
            "z", &T::z
    );
};

template<>
struct glz::meta<glm::quat> {
    using T = glm::quat;
    static constexpr auto value = glz::object(
            "x", &T::x,
            "y", &T::y,
            "z", &T::z,
            "w", &T::w
    );
};

template<>
struct glz::meta<glm::vec2> {
    using T = glm::vec2;
    static constexpr auto value = glz::object(
            "x", &T::x,
            "y", &T::y
    );
};

template<>
struct glz::meta<glm::mat4x4> {
    using T = glm::mat4x4;
    static constexpr auto value = glz::object(

    );
};





template<>
struct glz::meta<LittleCore::BoundingBox> {
    using T = LittleCore::BoundingBox;
    static constexpr auto value = glz::object(
            "center", &T::center,
            "extends", &T::extends
    );
};

template<typename TResource>
struct glz::meta<LittleCore::ResourceHandle<TResource>> {
    using T = LittleCore::ResourceHandle<TResource>;
    static constexpr auto value = glz::object(
    );
};