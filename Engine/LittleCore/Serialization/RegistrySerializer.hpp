//
// Created by Jeppe Nielsen on 01/02/2025.
//

#pragma once
#include "ComponentSerializerBase.hpp"
#include "ComponentDeserializer.hpp"
#include "CustomComponentDeserializer.hpp"
#include "TypeUtility.hpp"
#include "TupleHelper.hpp"
#include "SerializedRegistry.hpp"
#include <sstream>

namespace LittleCore {

    template<typename ...T>
    struct RegistrySerializer {

        template<typename S>
        static auto FindCustomSerializers() {
            if constexpr (CustomSerializer<S>) {
                return std::tuple<S>();
            } else {
                return std::tuple<>();
            }
        }

        static constexpr auto GetAllCustomSerializers() {
            return std::tuple_cat(FindCustomSerializers<T>()...);
        }

        using CustomSerializers = decltype(GetAllCustomSerializers());

        CustomSerializers customSerializers;
        std::tuple<T*...> typesToSerialize;

        std::unordered_map<std::string, std::unique_ptr<IComponentDeserializer>> deserializers;

        RegistrySerializer() {
            LittleCore::TupleHelper::for_each(typesToSerialize, [&] (auto typeToSerializePtr) {
                using ComponentType = typename std::remove_pointer_t<decltype(typeToSerializePtr)>;

                if constexpr (CustomSerializer<ComponentType>) {
                    std::string typeName = TypeUtility::GetClassName<typename ComponentType::Component>();
                    deserializers[typeName] = std::make_unique<CustomComponentDeserializer<ComponentType>>(std::get<ComponentType>(customSerializers));
                } else {
                    std::string typeName = TypeUtility::GetClassName<ComponentType>();
                    deserializers[typeName] = std::make_unique<ComponentDeserializer<ComponentType>>();
                }
            });
        }

        bool Serialize(const entt::registry& registry, std::ostream& stream) {
            SerializedRegistry<T...> serializedRegistry;

            LittleCore::TupleHelper::for_each(typesToSerialize, [&] (auto typeToSerializePtr) {
                using TypeToSerialize = typename std::remove_pointer_t<decltype(typeToSerializePtr)>;

                if constexpr (CustomSerializer<TypeToSerialize>) {
                    using SerializedComponentType = TypeToSerialize::SerializedComponent;
                    using ComponentType = TypeToSerialize::Component;
                    using ComponentList = SerializedComponentList<SerializedComponentType, ComponentType>;
                    ComponentList& componentList = std::get<ComponentList>(serializedRegistry.components);
                    auto& customSerializer = std::get<TypeToSerialize>(customSerializers);

                    for(const auto& [entity, component] : registry.view<ComponentType>().each()) {
                        SerializedComponent<SerializedComponentType> comp;
                        std::get<0>(comp) = (uint32_t)entity;
                        customSerializer.Serialize(component, std::get<1>(comp));
                        componentList.components.emplace_back(comp);
                    }

                } else {

                    using ComponentList = SerializedComponentPtrList<TypeToSerialize, TypeToSerialize>;

                    auto& componentList = std::get<ComponentList>(serializedRegistry.components);

                    for (const auto& [entity, component]: registry.view<TypeToSerialize>().each()) {

                        SerializedComponentPtr<TypeToSerialize> comp;
                        std::get<0>(comp) = (uint32_t) entity;
                        std::get<1>(comp) = &component;

                        componentList.components.emplace_back(comp);
                    }
                }
            });

            std::string buffer;
            auto error = glz::write<glz::opts{.prettify = true}>(serializedRegistry, buffer);

            if (error) {
                return false;
            }

            stream << buffer;
            return true;
        }

        bool Deserialize(const std::istream& stream, entt::registry& registry) {
            std::stringstream buffer;
            buffer << stream.rdbuf();
            glz::json_t json;
            auto error = glz::read_json(json, buffer.str());

            if (error) {
                return false;
            }

            auto& obj = json.get_object();
            if (!obj.contains("components")) {
                return false;
            }

            const auto& componentTypes = obj["components"].get_array();

            for(const auto& componentType : componentTypes) {
                if (!componentType.contains("type")) {
                    continue;
                }

                const auto& componentTypeId = componentType["type"].get_string();

                const auto& deserializer = deserializers.find(componentTypeId);

                if (deserializer == deserializers.end()) {
                    continue;
                }

                if (!componentType.contains("components")) {
                    return false;
                }

                const auto& componentElements = componentType["components"].get_array();

                deserializer->second->Deserialize(componentElements, registry);
            }

            return true;
        }
    };

}