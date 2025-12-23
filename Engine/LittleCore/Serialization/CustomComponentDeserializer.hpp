//
// Created by Jeppe Nielsen on 01/02/2025.
//

#pragma once
#include "IComponentDeserializer.hpp"
#include "RegistrySerializerConcepts.hpp"

namespace LittleCore {

    template<typename TCustomSerializer>
    struct CustomComponentDeserializer : public IComponentDeserializer {
        TCustomSerializer& customSerializer;

        CustomComponentDeserializer(TCustomSerializer& customSerializer)
        : customSerializer(customSerializer) { }

        glz::error_ctx Deserialize(const std::vector<glz::generic>& components, entt::registry& registry, glz::context& context) override {
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
                auto error = glz::read_json(serializedComponent, componentJson);

                if (error) {
                    return error;
                }

                TComponent& componentData = registry.emplace<TComponent>(entityId);
                customSerializer.Deserialize(serializedComponent, componentData);
            }

            return {};
        }

        glz::error_ctx DeserializeComponent(entt::registry& registry, entt::entity entity, const std::string& json, glz::context& context) override {
            using TComponent = TCustomSerializer::Component;
            TComponent& componentData = registry.get<TComponent>(entity);
            auto error = glz::read<glz::opts{}>(componentData, json, context);
            if (error) {
                return error;
            }
            return {};
        }

        std::string SerializeComponent(const entt::registry& registry, entt::entity entity, glz::context& context) override {
            using TComponent = TCustomSerializer::Component;
            using TSerializedComponent = TCustomSerializer::SerializedComponent;
            if (registry.all_of<TComponent>(entity)) {
                return "";
            }

            const TComponent& componentData = registry.get<TComponent>(entity);

            TSerializedComponent serializedComponent;

            if constexpr (HasSerializeWithRegistryAndEntity<TCustomSerializer, TComponent, TSerializedComponent>) {
                customSerializer.Serialize(componentData, serializedComponent, registry, entity);
            } else {
                customSerializer.Serialize(componentData, serializedComponent);
            }

            std::string json;
            auto error = glz::write<glz::opts{.prettify = true}>(serializedComponent, json, context);
            return json;
        }

        bool EntityHasComponent(const entt::registry& registry, entt::entity entity) override {
            using TComponent = TCustomSerializer::Component;
            return registry.all_of<TComponent>(entity);
        }
    };

}