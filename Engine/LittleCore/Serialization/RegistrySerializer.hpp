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
#include "IgnoreSerialization.hpp"
#include "ComponentReflection.hpp"
#include "RegistrySerializerConcepts.hpp"
#include "RegistrySerializerBase.hpp"

namespace LittleCore {

    template<typename ...T>
    struct RegistrySerializer : public RegistrySerializerBase {

        template<typename S>
        static auto FindCustomSerializers() {
            if constexpr (CustomSerializerPredicate<S>) {
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


        template<typename S>
        static auto FindCustomSerializerComponentTypes() {
            if constexpr (CustomSerializerPredicate<S>) {
                return std::tuple<typename S::Component>();
            } else {
                return std::tuple<>();
            }
        }

        template<typename S>
        static auto FindCustomSerializerComponentTypesPtr() {
            if constexpr (CustomSerializerPredicate<S>) {
                return std::tuple<typename S::Component*>();
            } else {
                return std::tuple<>();
            }
        }

        template<typename S>
        static auto FindNonCustomSerializerComponentTypesPtr() {
            if constexpr (NonCustomSerializerPredicate<S>) {
                return std::tuple<S*>();
            } else {
                return std::tuple<>();
            }
        }

        static constexpr auto GetAllCustomSerializerComponentTypes() {
            return std::tuple_cat(FindCustomSerializerComponentTypes<T>()...);
        }

        static constexpr auto GetAllCustomSerializerComponentTypesPtr() {
            return std::tuple_cat(FindCustomSerializerComponentTypesPtr<T>()...);
        }

        static constexpr auto GetAllNonCustomSerializerComponentTypesPtr() {
            return std::tuple_cat(FindNonCustomSerializerComponentTypesPtr<T>()...);
        }

        static constexpr auto GetAllTypes() {
            return std::tuple<T...>();
        }

        using AllNonCustomSerializerComponentTypesPtr = decltype(LittleCore::TupleHelper::exclude(GetAllNonCustomSerializerComponentTypesPtr(), GetAllCustomSerializerComponentTypesPtr()));
        AllNonCustomSerializerComponentTypesPtr allNonCustomSerializerComponentTypesPtr;

        using AllComponentTypes = decltype(LittleCore::TupleHelper::exclude(GetAllTypes(), GetAllCustomSerializerComponentTypes()));

        std::unordered_map<std::string, std::unique_ptr<IComponentDeserializer>> deserializers;

        template<typename TSerializer>
        TSerializer& GetSerializer() {
            return std::get<TSerializer>(customSerializers);
        }

        RegistrySerializer() {

            LittleCore::TupleHelper::for_each(customSerializers, [&] (auto& customSerializer) {
                using ComponentType = typename std::remove_cvref_t<decltype(customSerializer)>;
                std::string typeName = TypeUtility::GetClassName<typename ComponentType::Component>();
                deserializers[typeName] = std::make_unique<CustomComponentDeserializer<ComponentType>>(std::get<ComponentType>(customSerializers));
            });

            LittleCore::TupleHelper::for_each(allNonCustomSerializerComponentTypesPtr, [&] (auto typeToSerializePtr) {
                using ComponentType = typename std::remove_pointer_t<decltype(typeToSerializePtr)>;
                std::string typeName = TypeUtility::GetClassName<ComponentType>();
                deserializers[typeName] = std::make_unique<ComponentDeserializer<ComponentType>>();
            });
        }


        template <typename Tuple>
        struct ToRegistry;

        template <typename... Ts>
        struct ToRegistry<std::tuple<Ts...>> {
            using type = SerializedRegistry<Ts...>;
        };


        template <typename G>
        struct type_printer;

        template <typename G>
        void print_type_compiletime() {
            type_printer<G> dummy;  // Will trigger error showing `T`
        }

        std::string Serialize(const entt::registry& registry, SerializationContext& context) override {

            context.registrySerializer = this;
            context.registry = (entt::registry *)&registry;

            using SerializedRegistryType = ToRegistry<AllComponentTypes>::type;
            SerializedRegistryType serializedRegistry;

            LittleCore::TupleHelper::for_each(customSerializers, [&] (auto& customSerializer) {
                using TypeToSerialize = typename std::remove_cvref_t<decltype(customSerializer)>;

                using SerializedComponentType = TypeToSerialize::SerializedComponent;
                using ComponentType = TypeToSerialize::Component;
                using ComponentList = SerializedComponentList<SerializedComponentType, ComponentType>;
                ComponentList& componentList = std::get<ComponentList>(serializedRegistry.components);

                for (const auto& [entity, component]: registry.view<ComponentType>().each()) {
                    if (registry.any_of<IgnoreSerialization>(entity)) {
                        continue;
                    }
                    SerializedComponent<SerializedComponentType> comp;
                    std::get<0>(comp) = (uint32_t) entity;

                    if constexpr (HasSerializeWithRegistryAndEntity<TypeToSerialize, ComponentType, SerializedComponentType>) {
                        customSerializer.Serialize(component, std::get<1>(comp), registry, entity);
                    } else {
                        customSerializer.Serialize(component, std::get<1>(comp));
                    }
                    componentList.components.emplace_back(comp);
                }
            });

            LittleCore::TupleHelper::for_each(allNonCustomSerializerComponentTypesPtr, [&] (auto typeToSerializePtr) {
                using TypeToSerialize = typename std::remove_pointer_t<decltype(typeToSerializePtr)>;

                using ComponentList = SerializedComponentPtrList<TypeToSerialize, TypeToSerialize>;

                auto& componentList = std::get<ComponentList>(serializedRegistry.components);

                for (const auto& [entity, component]: registry.view<TypeToSerialize>().each()) {
                    if (registry.any_of<IgnoreSerialization>(entity)) {
                        continue;
                    }
                    SerializedComponentPtr<TypeToSerialize> comp;
                    std::get<0>(comp) = (uint32_t) entity;
                    std::get<1>(comp) = &component;

                    componentList.components.emplace_back(comp);
                }
            });

            glz::context ctx;
            ctx.userData = &context;
            std::string jsonString;
            auto error = glz::write<glz::opts{.prettify = true}>(serializedRegistry, jsonString, ctx);
            return jsonString;
        }

        std::string Deserialize(entt::registry& registry, const std::string& jsonString, SerializationContext& context) override {

            context.registrySerializer = this;
            context.registry = &registry;

            glz::context ctx {.userData = &context};
            glz::generic json;
            auto error = glz::read<glz::opts{}>(json, jsonString, ctx);

            if (error) {
                return glz::format_error(error, jsonString);
            }

            auto& obj = json.get_object();
            if (!obj.contains("components")) {
                error = glz::error_ctx(glz::error_code::missing_key, "Missing components key");
                return glz::format_error(error, jsonString);
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
                    error = glz::error_ctx(glz::error_code::missing_key, componentTypeId + ": Missing components key");
                    return glz::format_error(error, jsonString);
                }

                const auto& componentElements = componentType["components"].get_array();

                auto error = deserializer->second->Deserialize(componentElements, registry, ctx);
                if (error) {
                    return glz::format_error(error, jsonString);
                }
            }

            return {};
        }

        std::string SerializeComponent(const entt::registry& registry, entt::entity entity, const std::string& componentTypeId, glz::context& context) override {
            const auto& deserializer = deserializers.find(componentTypeId);
            if (deserializer == deserializers.end()) {
                return "";
            }
            return deserializer->second->SerializeComponent(registry, entity, context);
        }

        glz::error_ctx DeserializeComponent(entt::registry& registry, entt::entity entity, const std::string& componentTypeId, const std::string& json, glz::context& context) override {
            const auto& deserializer = deserializers.find(componentTypeId);
            if (deserializer == deserializers.end()) {
                return {glz::error_code::none};
            }
            return deserializer->second->DeserializeComponent(registry, entity, json, context);
        }

        bool HasComponent(entt::registry& registry, entt::entity entity, const std::string& componentTypeId) override {
            const auto& deserializer = deserializers.find(componentTypeId);
            if (deserializer == deserializers.end()) {
                return false;
            }
            return deserializer->second->EntityHasComponent(registry, entity);
        }

    };

}