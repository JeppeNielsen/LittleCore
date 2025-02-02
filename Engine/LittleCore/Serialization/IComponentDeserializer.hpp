//
// Created by Jeppe Nielsen on 01/02/2025.
//

#pragma once
#include <glaze/glaze.hpp>
#include <entt/entt.hpp>

namespace LittleCore {

    struct IComponentDeserializer {
        virtual ~IComponentDeserializer() = default;
        virtual glz::error_ctx Deserialize(const std::vector<glz::json_t>& components, entt::registry& registry) = 0;
    };

}