//
// Created by Jeppe Nielsen on 26/12/2025.
//

#pragma once
#include <entt/entt.hpp>
#include <glaze/glaze.hpp>

namespace LittleCore {

    struct SerializationContext;

    struct RegistrySerializerBase {
        virtual ~RegistrySerializerBase() = default;
        virtual std::string Serialize(const entt::registry& registry, SerializationContext& context) = 0;
        virtual std::string Deserialize(entt::registry& registry, const std::string& jsonString, SerializationContext& context) = 0;
        virtual std::string SerializeComponent(const entt::registry& registry, entt::entity entity, const std::string& componentTypeId, glz::context& context) = 0;
        virtual glz::error_ctx DeserializeComponent(entt::registry& registry, entt::entity entity, const std::string& componentTypeId, const std::string& json, glz::context& context) = 0;
        virtual bool HasComponent(entt::registry& registry, entt::entity entity, const std::string& componentTypeId) = 0;
    };


}