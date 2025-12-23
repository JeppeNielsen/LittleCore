//
// Created by Jeppe Nielsen on 01/02/2025.
//

#pragma once
#include <glaze/glaze.hpp>
#include <entt/entt.hpp>
#include "SerializationContext.hpp"

namespace LittleCore {

    struct IComponentDeserializer {
        virtual ~IComponentDeserializer() = default;
        virtual glz::error_ctx Deserialize(const std::vector<glz::generic>& components, entt::registry& registry, glz::context& context) = 0;
        virtual glz::error_ctx DeserializeComponent(entt::registry& registry, entt::entity entity, const std::string& json, glz::context& context) = 0;
        virtual std::string SerializeComponent(const entt::registry& registry, entt::entity entity, glz::context& context) = 0;
        virtual bool EntityHasComponent(const entt::registry& registry, entt::entity entity) = 0;
    };

}