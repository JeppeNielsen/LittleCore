#include <iostream>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Example components
struct Position {
    float x, y;
};

struct Velocity {
    float dx, dy;
};

// Serialization class
class ComponentSerializer {
public:
    // Serialize Position component
    void to_json(json &j, const Position &p) {
        j = json{{"x", p.x}, {"y", p.y}};
    }

    // Deserialize Position component
    void from_json(const json &j, Position &p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
    }

    // Serialize Velocity component
    void to_json(json &j, const Velocity &v) {
        j = json{{"dx", v.dx}, {"dy", v.dy}};
    }

    // Deserialize Velocity component
    void from_json(const json &j, Velocity &v) {
        j.at("dx").get_to(v.dx);
        j.at("dy").get_to(v.dy);
    }
};

// Serialize the registry
json serializeRegistry(const entt::registry &registry, ComponentSerializer &serializer) {
    json output;

    const auto* storage = registry.storage<entt::entity>();
    for (auto entity : *storage) {
        json entityData;

        if (registry.all_of<Position>(entity)) {
            const auto &pos = registry.get<Position>(entity);
            serializer.to_json(entityData["Position"], pos);
        }
        if (registry.all_of<Velocity>(entity)) {
            const auto &vel = registry.get<Velocity>(entity);
            serializer.to_json(entityData["Velocity"], vel);
        }

        // Save the entity's data if it has any components
        if (!entityData.empty()) {
            output[std::to_string(static_cast<uint32_t>(entity))] = entityData;
        }
    };

    return output;
}

// Deserialize the registry
void deserializeRegistry(entt::registry &registry, const json &input, ComponentSerializer &serializer) {
    for (const auto &[key, value] : input.items()) {
        auto entity = static_cast<entt::entity>(std::stoul(key));
        if (value.contains("Position")) {
            Position pos;
            serializer.from_json(value["Position"], pos);
            registry.emplace<Position>(entity, pos);
        }
        if (value.contains("Velocity")) {
            Velocity vel;
            serializer.from_json(value["Velocity"], vel);
            registry.emplace<Velocity>(entity, vel);
        }
    }
}

int main_old() {
    entt::registry registry;
    ComponentSerializer serializer;

    // Create some entities and assign components
    auto entity1 = registry.create();
    registry.emplace<Position>(entity1, 10.0f, 20.0f);
    registry.emplace<Velocity>(entity1, 1.0f, 1.5f);

    auto entity2 = registry.create();
    registry.emplace<Position>(entity2, 30.0f, 40.0f);

    // Serialize the registry
    json serializedData = serializeRegistry(registry, serializer);
    std::cout << "Serialized Data:\n" << serializedData.dump(4) << "\n";

    // Deserialize the registry into a new registry
    entt::registry newRegistry;
    deserializeRegistry(newRegistry, serializedData, serializer);

    // Verify deserialization
    auto& storage = registry.storage<entt::entity>();
    for (auto entity : storage) {
        if (newRegistry.all_of<Position>(entity)) {
            auto &pos = newRegistry.get<Position>(entity);
            std::cout << "Entity " << static_cast<uint32_t>(entity) << " Position: " << pos.x << ", " << pos.y << "\n";
        }
        if (newRegistry.all_of<Velocity>(entity)) {
            auto &vel = newRegistry.get<Velocity>(entity);
            std::cout << "Entity " << static_cast<uint32_t>(entity) << " Velocity: " << vel.dx << ", " << vel.dy << "\n";
        }
    };

    return 0;
}
