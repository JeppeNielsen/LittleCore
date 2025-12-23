//
// Created by Jeppe Nielsen on 01/02/2025.
//

#pragma once
#include "IComponentDeserializer.hpp"
#include "ResourceSerialization.hpp"

namespace LittleCore {

    template<typename TComponent>
    struct ComponentDeserializer : public IComponentDeserializer {
        glz::error_ctx Deserialize(const std::vector<glz::generic>& components, entt::registry& registry, glz::context& context) override {

            for (const auto& component: components) {
                auto componentElement = component.get_array();
                entt::entity entityId = (entt::entity)componentElement[0].as<int>();
                auto componentJson = componentElement[1];
                if (!registry.valid(entityId)) {
                    entityId = registry.create(entityId);
                }

                TComponent& componentData = registry.emplace<TComponent>(entityId);
                auto error = glz::read_json(componentData, componentJson, context);
                if (error) {
                    return error;
                }
            }
            return {};
        }

        glz::error_ctx DeserializeComponent(entt::registry& registry, entt::entity entity, const std::string& json, glz::context& context) override {
            TComponent& componentData = registry.get<TComponent>(entity);
            auto error = glz::read<glz::opts{}>(componentData, json, context);
            if (error) {
                return error;
            }
            return {};
        }

        std::string SerializeComponent(const entt::registry& registry, entt::entity entity, glz::context& context) override {
            if (!registry.all_of<TComponent>(entity)) {
                return "";
            }

            const TComponent& component = registry.get<TComponent>(entity);

            std::string json;
            auto error = glz::write<glz::opts{.prettify = true}>(component, json, context);
            return json;
        }

        bool EntityHasComponent(const entt::registry& registry, entt::entity entity) override {
            return registry.all_of<TComponent>(entity);
        }
    };

}