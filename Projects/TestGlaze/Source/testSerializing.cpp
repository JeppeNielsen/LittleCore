//
// Created by Jeppe Nielsen on 29/01/2025.
//


#include <glaze/json.hpp>
#include <iostream>
#include <tuple>
#include <memory>
#include <TypeUtility.hpp>
#include "TupleHelper.hpp"
#include <entt/entt.hpp>

using namespace LittleCore;

template<typename TComponent, typename TSerializedComponent>
struct ComponentSerializerBase {
    using Component = TComponent;
    using SerializedComponent = TSerializedComponent;
    void CanSerialize() { };
};

template <typename T>
concept CustomSerializer = requires(T t) {
    { t.CanSerialize() } -> std::same_as<void>;
};



template<typename T>
using SerializedComponentPtr = std::tuple<uint32_t, const T*>;

template<typename T>
using SerializedComponent = std::tuple<uint32_t, T>;

template<typename T, typename TypeName>
struct SerializedComponentPtrList {
    using ComponentType = T;
    std::string type = LittleCore::TypeUtility::GetClassName<TypeName>();
    std::vector<SerializedComponentPtr<T>> components;
};

template<typename T, typename TypeName>
struct SerializedComponentList {
    using ComponentType = T;
    std::string type = LittleCore::TypeUtility::GetClassName<TypeName>();
    std::vector<SerializedComponent<T>> components;
};

template<typename ...T>
struct SerializedRegistry {

    template<typename S>
    static constexpr auto Iterate() {
        if constexpr (CustomSerializer<S>) {
            return std::make_tuple(SerializedComponentList<typename S::SerializedComponent, typename S::Component>());
        } else {
            return std::make_tuple(SerializedComponentPtrList<S, S>());
        }
    }

    static constexpr auto GetComponents() {
        return std::tuple_cat(Iterate<T>()...);
    }

    decltype(GetComponents()) components;
};

template<typename ...T>
struct DeserializedRegistry {

    template<typename S>
    static constexpr auto Iterate() {
        if constexpr (CustomSerializer<S>) {
            return std::make_tuple(SerializedComponentList<typename S::SerializedComponent, typename S::Component>());
        } else {
            return std::make_tuple(SerializedComponentList<S, S>());
        }
    }

    static constexpr auto GetComponents() {
        return std::tuple_cat(Iterate<T>()...);
    }

    decltype(GetComponents()) components;
};




template<typename ...T>
struct ComponentList;

template<typename ...T>
struct SerializedRegistryFactory {

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

    SerializedRegistry<T...> Create(const entt::registry& registry) {
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


        return serializedRegistry;
    }

    DeserializedRegistry<T...> CreateDeserialized()



};



struct Renderable {
    int shaderId;
};

struct Transform {
    int x;
    int y;
};

struct Velocity {
    float vx;
    float vy;
};

struct Texturable {
    int textureId;
};

struct SerializableTexturable {
    std::string path;
};

struct TexturableSerializer : ComponentSerializerBase<Texturable, SerializableTexturable> {

    void Serialize(const Texturable& texturable, SerializableTexturable& serializableTexturable) {
        if (texturable.textureId == 5) {
            serializableTexturable.path = "Image.png";
        } else {
            serializableTexturable.path = "Ship.png";
        }
    }

    void Deserialize(const SerializableTexturable& serializableComponent, Texturable& texturable) {
        if (serializableComponent.path == "Image.png") {
            texturable.textureId = 5;
        } else {
            texturable.textureId = 0;
        }
    }


};

//using DefaultDeserializedRegistry = DeserializedRegistry<Renderable, Transform, Velocity>;

using DefaultSerializedRegistryFactory = SerializedRegistryFactory<Renderable, Transform, TexturableSerializer>;

using Objects = std::tuple<Renderable, Transform>;

int main() {

    entt::registry registry;
    auto entity = registry.create();
    registry.emplace<Renderable>(entity);
    registry.emplace<Transform>(entity);
    registry.emplace<Texturable>(entity);

    auto entity2 = registry.create();
    registry.emplace<Renderable>(entity2);
    registry.emplace<Texturable>(entity2).textureId = 5;

    DefaultSerializedRegistryFactory factory;

    auto serialized = factory.Create(registry);

    std::string jsonBuffer;
    glz::write<glz::opts{.prettify = true}>(serialized, jsonBuffer);

    std::cout << jsonBuffer << "\n";
/*

    DefaultDeserializedRegistry deserialized;

    auto error = glz::read_json(deserialized, jsonBuffer);

    if (error) {
        std::string descriptive_error = glz::format_error(error, jsonBuffer);
        std::cout << descriptive_error << "\n";
    }

    std::string pretty_json;
    glz::write<glz::opts{.prettify = true}>(deserialized, pretty_json);
    std::cout << pretty_json << "\n";

    */
    return 0;
}