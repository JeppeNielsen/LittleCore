//
// Created by Jeppe Nielsen on 30/10/2025.
//

#pragma once
#include <entt/entt.hpp>
#include <vector>
#include <string>
#include <glaze/glaze.hpp>
#include "RegistrySerializerBase.hpp"

namespace LittleCore {
    struct SerializedPrefabComponent {
        entt::entity sourceEntity;
        entt::entity entity;
        std::string componentId;
        std::string data;
    };
}

template<>
struct glz::meta<LittleCore::SerializedPrefabComponent> {
    using T = LittleCore::SerializedPrefabComponent;

    static constexpr auto read = [](T& self, const std::string& data, glz::context& context) {
        self.data = data;
    };

    static constexpr auto write = [](const T& self, glz::context& context) -> std::string {
        LittleCore::SerializationContext* c = static_cast<LittleCore::SerializationContext*>(context.userData);
        return c->registrySerializer->SerializeComponent(*c->registry, self.entity, self.componentId, context);
    };

    static constexpr auto value = glz::object(
        "sourceEntity", &T::sourceEntity,
        "componentId", &T::componentId,
        "data", glz::custom<read, write>
    );
};

