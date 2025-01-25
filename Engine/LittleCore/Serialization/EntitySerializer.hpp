//
// Created by Jeppe Nielsen on 24/01/2025.
//

#pragma once
#include <nlohmann/json.hpp>
#include <tuple>
#include "TupleHelper.hpp"
#include <entt/entt.hpp>
#include "TypeUtility.hpp"

namespace LittleCore {
    template<typename ...T>
    class EntitySerializer {
    public:
        using ComponentSerializers = std::tuple<T...>;

        template<typename TSerializer>
        TSerializer& Get() {
            return std::get<TSerializer>(componentSerializers);
        }

        void SerializeEntity(nlohmann::json& json, const entt::registry& registry, entt::entity entity) {
            nlohmann::json entityData;

            bool didSerializeAnything = false;
            TupleHelper::for_each(componentSerializers, [&] (auto& type) {
                using ComponentType = typename std::remove_reference_t<decltype(type)>::ComponentType;
                if (registry.all_of<ComponentType>(entity)) {
                    const std::string componentName = TypeUtility::GetClassName<ComponentType>();
                    nlohmann::json& componentJson = entityData[componentName];
                    const auto &component = registry.get<ComponentType>(entity);
                    type.Serialize(componentJson, component);
                    didSerializeAnything = true;
                }
            });

            if (!entityData.empty()) {
                json[std::to_string(static_cast<uint32_t>(entity))] = entityData;
            }
        }

        void DeserializeEntity(const nlohmann::json& json, entt::registry& registry, entt::entity entity) {
            TupleHelper::for_each(componentSerializers, [&] (auto& type) {
                using ComponentType = typename std::remove_reference_t<decltype(type)>::ComponentType;
                const std::string componentName = TypeUtility::GetClassName<ComponentType>();
                if (json.contains(componentName)) {
                    ComponentType component = type.Deserialize(json[componentName]);
                    registry.emplace<ComponentType>(entity, component);
                }
            });
        }

        ComponentSerializers componentSerializers;
    };
}