//
// Created by Jeppe Nielsen on 24/01/2025.
//


#pragma once
#include <nlohmann/json.hpp>

namespace LittleCore {
    template<typename T>
    struct ComponentSerializer {
        using json = nlohmann::json;
        using ComponentType = T;

        virtual ~ComponentSerializer() = default;

        virtual void Serialize(json& json, const T& component) = 0;
        virtual T Deserialize(const json &j) = 0;

    };
}
