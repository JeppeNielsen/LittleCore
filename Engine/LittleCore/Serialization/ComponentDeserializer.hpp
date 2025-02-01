//
// Created by Jeppe Nielsen on 01/02/2025.
//

#pragma once
#include "IComponentDeserializer.hpp"

namespace LittleCore {

    template<typename TComponent>
    struct ComponentDeserializer : public IComponentDeserializer {
        void Deserialize(const std::vector<glz::json_t>& components, entt::registry& registry) {

            for (const auto& component: components) {
                auto componentElement = component.get_array();
                entt::entity entityId = (entt::entity)componentElement[0].as<int>();
                auto componentJson = componentElement[1];
                if (!registry.valid(entityId)) {
                    entityId = registry.create(entityId);
                }

                TComponent& componentData = registry.emplace<TComponent>(entityId);
                glz::read_json(componentData, componentJson);
            }
        }
    };

}