#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <ComponentSerializer.hpp>
#include "DefaultSimulation.hpp"
#include "ComponentSerializerRegistry.hpp"

using namespace LittleCore;


// Example components
struct Position {
    float x, y;

    // Serialization for nlohmann/json
    friend void to_json(nlohmann::json &j, const Position &p) {
        j = nlohmann::json{{"x", p.x}, {"y", p.y}};
    }

    friend void from_json(const nlohmann::json &j, Position &p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
    }
};

struct Velocity {
    float dx, dy;

    // Serialization for nlohmann/json
    friend void to_json(nlohmann::json &j, const Velocity &v) {
        j = nlohmann::json{{"dx", v.dx}, {"dy", v.dy}};
    }

    friend void from_json(const nlohmann::json &j, Velocity &v) {
        j.at("dx").get_to(v.dx);
        j.at("dy").get_to(v.dy);
    }
};


struct PositionSerializer : public ComponentSerializer<Position> {

    void Serialize(json& json, const Position& component) override {
        json = nlohmann::json{{"x", component.x}, {"y", component.y}};
    }

    Position Deserialize(const json &j) override {
        Position position;
        j.at("dx").get_to(position.x);
        j.at("dy").get_to(position.y);
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
    }

};

using DefaultComponentSerializerRegistry = ComponentSerializerRegistry<PositionSerializer, VelocitySerializer>;


// Serialize the entire registry to JSON
void serialize_registry(const entt::registry &registry, nlohmann::json &output) {
    output["entities"] = nlohmann::json::array();
    output["components"] = nlohmann::json::object();

    const auto& storage = registry.storage<entt::entity>();
    for (auto entity : *storage) {
        output["entities"].push_back(entity);
    }

    // Save components by type
    if (!registry.storage<Position>()->empty()) {
        nlohmann::json position_data = nlohmann::json::array();
        for (auto [entity, pos] : registry.view<Position>().each()) {
            position_data.push_back({{"entity", entity}, {"component", pos}});
        }
        output["components"]["Position"] = position_data;
    }

    if (!registry.storage<Velocity>()->empty()) {
        nlohmann::json velocity_data = nlohmann::json::array();
        for (auto [entity, vel] : registry.view<Velocity>().each()) {
            velocity_data.push_back({{"entity", entity}, {"component", vel}});
        }
        output["components"]["Velocity"] = velocity_data;
    }
}

// Deserialize the registry from JSON
void deserialize_registry(entt::registry &registry, const nlohmann::json &input) {
    registry.clear();

    // Restore entities
    for (const auto &entity_json : input["entities"]) {
        registry.create(entity_json);
    }

    // Restore components
    if (input["components"].contains("Position")) {
        for (const auto &entry : input["components"]["Position"]) {
            auto entity = entry["entity"].get<entt::entity>();
            auto pos = entry["component"].get<Position>();
            registry.emplace<Position>(entity, pos);
        }
    }

    if (input["components"].contains("Velocity")) {
        for (const auto &entry : input["components"]["Velocity"]) {
            auto entity = entry["entity"].get<entt::entity>();
            auto vel = entry["component"].get<Velocity>();
            registry.emplace<Velocity>(entity, vel);
        }
    }
}

int main() {

    DefaultComponentSerializerRegistry defaultComponentSerializerRegistry;
    
    entt::registry registry;
    std::ifstream input_file("registry.json");
    nlohmann::json input_json;
    input_file >> input_json;

    DefaultSimulation defaultSimulation(registry);

    try {
        deserialize_registry(registry, input_json);
        std::cout << "Parsing succes\n";
    } catch(std::exception& e) {
        std::cout << "Parsing failed" << e.what() << "\n";
    }

    // Verify deserialization
    std::cout << "Deserialized Entities:\n";
    //registry.each([](auto entity) {
    //    std::cout << "Entity: " << int(entity) << '\n';
    //});

    for (auto [entity, pos] : registry.view<Position>().each()) {
        std::cout << "Position: " << pos.x << ", " << pos.y << '\n';
    }

    for (auto [entity, vel] : registry.view<Velocity>().each()) {
        std::cout << "Velocity: " << vel.dx << ", " << vel.dy << '\n';
    }

}

int main_old() {
    entt::registry registry;

    // Create some entities and add components
    auto entity1 = registry.create();
    registry.emplace<Position>(entity1, 10.0f, 20.0f);
    registry.emplace<Velocity>(entity1, 1.0f, 1.5f);

    auto entity2 = registry.create();
    registry.emplace<Position>(entity2, 30.0f, 40.0f);

    // Serialize the registry to JSON
    nlohmann::json json_data;
    serialize_registry(registry, json_data);

    // Write JSON to a file
    std::ofstream file("registry.json");
    file << json_data.dump(4);
    file.close();

    // Print JSON to console
    std::cout << "Serialized JSON:\n" << json_data.dump(4) << "\n";

    // Clear and deserialize the registry from JSON
    std::ifstream input_file("registry.json");
    nlohmann::json input_json;
    input_file >> input_json;

    deserialize_registry(registry, input_json);

    // Verify deserialization
    std::cout << "Deserialized Entities:\n";
    //registry.each([](auto entity) {
    //    std::cout << "Entity: " << int(entity) << '\n';
    //});

    for (auto [entity, pos] : registry.view<Position>().each()) {
        std::cout << "Position: " << pos.x << ", " << pos.y << '\n';
    }

    return 0;
}
