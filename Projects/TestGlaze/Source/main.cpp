//
// Created by Jeppe Nielsen on 29/01/2025.
//


#include "RegistrySerializer.hpp"
#include <iostream>

using namespace LittleCore;

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

using DefaultRegistrySerializer = RegistrySerializer<Renderable, Transform, TexturableSerializer>;

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

    DefaultRegistrySerializer serializer;

    //std::stringstream buffer;
    //serializer.Serialize(registry, buffer);

    //std::ofstream file("registry.json");
    //file<<buffer.str();

    std::ifstream inputFile("registry.json");
    entt::registry deserializedRegistry;
    std::string jsonBuffer;
    auto error = serializer.Deserialize(inputFile, deserializedRegistry, jsonBuffer);
    if (error) {
        std::cout << "Deserialization failed: " << glz::format_error(error, jsonBuffer) <<"\n";
    }

    std::cout << "Deserialized Entities:\n";
    for (auto [entity, renderable] : deserializedRegistry.view<Renderable>().each()) {
        std::cout << "Renderable: shaderId = " << renderable.shaderId << "\n";
    }
    for (auto [entity, t] : deserializedRegistry.view<Transform>().each()) {
        std::cout << "Transform: x: " <<t.x << ", y:"<<t.y << "\n";
    }

    for (auto [entity, t] : deserializedRegistry.view<Texturable>().each()) {
        std::cout << "Texturable: textureId: " <<t.textureId << "\n";
    }


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