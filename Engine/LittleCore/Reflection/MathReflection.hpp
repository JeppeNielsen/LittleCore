//
// Created by Jeppe Nielsen on 18/09/2025.
//
#include <glm/glm.hpp>
#include <glaze/glaze.hpp>

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
