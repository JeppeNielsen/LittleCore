//
// Created by Jeppe Nielsen on 24/01/2025.
//

#pragma once
#include <glaze/json.hpp>
#include <tuple>
#include "TupleHelper.hpp"
#include <entt/entt.hpp>
#include "TypeUtility.hpp"

namespace LittleCore {

    template <typename T>
    concept CustomSerializer = requires(T t) {
        { t.CanSerialize() } -> std::same_as<void>;
    };

    template<typename ...T>
    class EntitySerializer {
    public:
        using ComponentSerializers = std::tuple<T...>;

        template<typename TSerializer>
        TSerializer& Get() {
            return std::get<TSerializer>(componentSerializers);
        }

        bool SerializeEntity(glz::json_t& json, const entt::registry& registry, entt::entity entity) {
            glz::json_t entityData;
            bool didSucceed = true;
            TupleHelper::for_each(componentSerializers, [&] (auto& type) {
                using Type = typename std::remove_reference_t<decltype(type)>;
                if constexpr (CustomSerializer<Type>) {
                    using ComponentType = typename std::remove_reference_t<decltype(type)>::ComponentType;
                    if (registry.all_of<ComponentType>(entity)) {
                        const std::string componentName = TypeUtility::GetClassName<ComponentType>();
                        auto& componentJson = entityData[componentName];
                        const auto &component = registry.get<ComponentType>(entity);
                        type.Serialize(componentJson, component);
                    }
                } else {
                    if (registry.all_of<Type>(entity)) {
                        const std::string componentName = TypeUtility::GetClassName<Type>();
                        const auto &component = registry.get<Type>(entity);
                        std::string jsonData;
                        auto error = glz::write_json(component, jsonData);
                        if (!error) {
                            entityData[componentName] = jsonData;
                        } else {
                            didSucceed = false;
                        }
                    }
                }
            });

            if (!entityData.empty()) {
                json[std::to_string(static_cast<uint32_t>(entity))] = entityData;
            }
            return didSucceed;
        }

        bool DeserializeEntity(const glz::json_t& json, entt::registry& registry, entt::entity entity) {
            bool didSucceed = true;
            TupleHelper::for_each(componentSerializers, [&] (auto& type) {
                using Type = typename std::remove_reference_t<decltype(type)>;
                if constexpr (CustomSerializer<Type>) {
                    using ComponentType = typename std::remove_reference_t<decltype(type)>::ComponentType;
                    const std::string componentName = TypeUtility::GetClassName<ComponentType>();

                    if (json.contains(componentName)) {
                        ComponentType& component = registry.emplace<ComponentType>(entity);
                        if (!type.Deserialize(json[componentName], component)) {
                            didSucceed = false;
                        }
                    }

                } else {
                    const std::string componentName = TypeUtility::GetClassName<Type>();

                    if (json.contains(componentName)) {
                        auto& jsonData = json[componentName].get_string();
                        Type& component = registry.emplace<Type>(entity);
                        auto error = glz::read_json(component, jsonData);
                        if (error) {
                            didSucceed = false;
                        }
                    }
                }
            });
            return didSucceed;
        }

        ComponentSerializers componentSerializers;
    };
}