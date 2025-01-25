#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <ComponentSerializer.hpp>
#include "DefaultSimulation.hpp"
#include "EntitySerializer.hpp"
#include "RegistrySerializer.hpp"

using namespace LittleCore;


// Example components
struct Position {
    float x, y;
};

struct Velocity {
    float dx, dy;
};

struct Children {
    std::vector<entt::entity> children;
};

struct ChildrenSerializer : public ComponentSerializer<Children> {

    void Serialize(json& json, const Children& component) override {
        json["children"] = component.children;
    }

    Children Deserialize(const json &j) override {
        Children children;
        children.children = j["children"].get<std::vector<entt::entity>>();
        return children;
    }

};


struct PositionSerializer : public ComponentSerializer<Position> {

    void Serialize(json& json, const Position& component) override {
        json = nlohmann::json{{"x", component.x}, {"y", component.y}};
    }

    Position Deserialize(const json &j) override {
        Position position;
        j.at("x").get_to(position.x);
        j.at("y").get_to(position.y);
        return position;
    }

};

struct VelocitySerializer : public ComponentSerializer<Velocity> {

    void Serialize(json& json, const Velocity& component) override {
        json = nlohmann::json{{"dx", component.dx}, {"dy", component.dy}};
    }

    Velocity Deserialize(const json &j) override {
        Velocity velocity;
        j.at("dx").get_to(velocity.dx);
        j.at("dy").get_to(velocity.dy);
        return velocity;
    }

};

using DefaultEntitySerializer = EntitySerializer<PositionSerializer, VelocitySerializer, ChildrenSerializer>;

int main() {
    entt::registry registry;

    // Create some entities and add components
    auto entity1 = registry.create();
    registry.emplace<Position>(entity1, 10.0f, 20.0f);
    registry.emplace<Velocity>(entity1, 5.0f, 1.5f);


    auto entity2 = registry.create();
    registry.emplace<Position>(entity2, 30.0f, 40.0f);

    auto& c = registry.emplace<Children>(entity1);
            c.children.push_back(entity2);
            c.children.push_back(entity1);

    DefaultEntitySerializer defaultEntitySerializer;

    RegistrySerializer<DefaultEntitySerializer> registrySerializer(defaultEntitySerializer);

    //std::ofstream file("registry.json");
    //registrySerializer.Serialize(file, registry);
    //file.close();

    // Clear and deserialize the registry from JSON
    std::ifstream input_file("registry.json");

    entt::registry newRegistry;
    registrySerializer.Deserialize(input_file, newRegistry);

    //deserialize_registry(registry, input_json);

    std::cout << "Serialized Entities:\n";
    for (auto [entity, pos] : registry.view<Position>().each()) {
        std::cout << "Position: " << pos.x << ", " << pos.y << '\n';
    }

    // Verify deserialization
    std::cout << "Deserialized Entities:\n";
    for (auto [entity, pos] : newRegistry.view<Position>().each()) {
        std::cout << "Position: " << pos.x << ", " << pos.y << '\n';
    }
    for (auto [entity, c] : newRegistry.view<Children>().each()) {
        std::cout << "Children: " << c.children.size() <<"\n";
    }


    return 0;
}
