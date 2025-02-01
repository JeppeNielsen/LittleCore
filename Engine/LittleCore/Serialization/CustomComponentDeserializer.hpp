//
// Created by Jeppe Nielsen on 01/02/2025.
//

#pragma once
#include "IComponentDeserializer.hpp"

namespace LittleCore {

    template<typename TCustomSerializer>
    struct CustomComponentDeserializer : public IComponentDeserializer {
        TCustomSerializer& customSerializer;

        CustomComponentDeserializer(TCustomSerializer& customSerializer)
        : customSerializer(customSerializer) { }

        void Deserialize(const std::vector<glz::json_t>& components, entt::registry& registry) {
            using TComponent = TCustomSerializer::Component;
            using TSerializedComponent = TCustomSerializer::SerializedComponent;

            for (const auto& component: components) {
                auto componentElement = component.get_array();
                entt::entity entityId = (entt::entity)componentElement[0].as<int>();
                auto componentJson = componentElement[1];
                if (!registry.valid(entityId)) {
                    entityId = registry.create(entityId);
                }
                TSerializedComponent serializedComponent;
                glz::read_json(serializedComponent, componentJson);

                TComponent& componentData = registry.emplace<TComponent>(entityId);

                customSerializer.Deserialize(serializedComponent, componentData);
            }
        }
    };

}