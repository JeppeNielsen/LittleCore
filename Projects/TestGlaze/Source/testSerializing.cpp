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

/*
template<typename T>
struct SerializedComponent {
    const uint32_t e;
    const T* component;
};
 */

template<typename T>
using SerializedComponent = std::tuple<uint32_t, const T*>;

template<typename T>
struct SerializedComponentList {
    using ComponentType = T;
    std::string type = LittleCore::TypeUtility::GetClassName<T>();
    std::vector<SerializedComponent<T>> components;
};

template<typename ...T>
struct SerializedRegistry {

    std::tuple<SerializedComponentList<T>...> components;

};

template<typename T>
using DeserializedComponent = std::tuple<uint32_t, T>;

template<typename T>
struct DeserializedComponentList {
    using ComponentType = T;
    std::string type = LittleCore::TypeUtility::GetClassName<T>();
    std::vector<DeserializedComponent<T>> components;
};

template<typename ...T>
struct DeserializedRegistry {

    std::tuple<DeserializedComponentList<T>...> components;

};


template<typename T>
struct SerializedRegistryFactory {


    T Create(const entt::registry& registry) {
        T serializedRegistry;

        LittleCore::TupleHelper::for_each(serializedRegistry.components, [&] (auto& componentList) {
            using ComponentListType = typename std::remove_reference_t<decltype(componentList)>;
            using ComponentType = ComponentListType::ComponentType;
            for(const auto& [entity, component] : registry.view<ComponentType>().each()) {
                SerializedComponent<ComponentType> comp;
                std::get<0>(comp) = (uint32_t)entity;
                std::get<1>(comp) = &component;

                componentList.components.push_back(comp);
            }

        });


        return serializedRegistry;
    }


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

using DefaultSerializedRegistry = SerializedRegistry<Renderable, Transform, Velocity>;

using DefaultDeserializedRegistry = DeserializedRegistry<Renderable, Transform, Velocity>;

using DefaultSerializedRegistryFactory = SerializedRegistryFactory<DefaultSerializedRegistry>;


int main() {

    entt::registry registry;
    auto entity = registry.create();
    registry.emplace<Renderable>(entity);
    registry.emplace<Transform>(entity);

    auto entity2 = registry.create();
    registry.emplace<Renderable>(entity2);

    DefaultSerializedRegistryFactory factory;

    auto serialized = factory.Create(registry);


    std::string jsonBuffer;
    glz::write<glz::opts{.prettify = true}>(serialized, jsonBuffer);

    std::cout << jsonBuffer << "\n";

    DefaultDeserializedRegistry deserialized;

    auto error = glz::read_json(deserialized, jsonBuffer);

    if (error) {
        std::string descriptive_error = glz::format_error(error, jsonBuffer);
        std::cout << descriptive_error << "\n";
    }

    std::string pretty_json;
    glz::write<glz::opts{.prettify = true}>(deserialized, pretty_json);
    std::cout << pretty_json << "\n";
    return 0;
}