//
// Created by Jeppe Nielsen on 24/01/2025.
//


#pragma once
#include <glaze/json.hpp>

namespace LittleCore {
    template<typename T>
    struct ComponentSerializer {
        using ComponentType = T;

        virtual ~ComponentSerializer() = default;

        void CanSerialize() {}

        virtual bool Serialize(glz::json_t& json, const T& component) = 0;
        virtual bool Deserialize(const glz::json_t &json, T& component) = 0;

    };
}
