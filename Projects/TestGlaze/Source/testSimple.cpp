//
// Created by Jeppe Nielsen on 27/01/2025.
//


#include <iostream>
#include <string>
#include <glaze/glaze.hpp>
#include <tuple>
#include <vector>
#include <entt/entt.hpp>
#include <sstream>
#include "TypeUtility.hpp"

struct Pos {
    float x;
    float y;
};

struct Vel {
    float vx;
    float vy;
};

template<typename T>
struct Component {
    int entity;
    T data;
    std::string name = LittleCore::TypeUtility::GetClassName<T>();
};

template<typename ...T>
struct RegistryTest {

    std::tuple<std::vector<Component<T>>...> components;
};

using Registry = RegistryTest<Pos, Vel>;

int main_old2() {

    entt::registry registry;

    // Add some entities and components
    auto entity1 = registry.create();
    registry.emplace<Pos>(entity1, 1.0f, 2.0f);
    registry.emplace<Vel>(entity1, 0.5f, 0.5f);

    auto entity2 = registry.create();
    registry.emplace<Pos>(entity2, 3.0f, 4.0f);

    // Example JSON string
    std::string jsonString;

    Registry test;
    std::get<std::vector<Component<Pos>>>(test.components).push_back({3,6, 5});

    // Parse the JSON string into the glz::json_t object
    auto error = glz::write_json(test, jsonString);

    // Check for errors
    if (error) {
        std::cerr << "Error parsing JSON string into glz::json_t: " << error << std::endl;
        return 1;
    }

    // Output the glz::json_t structure
    std::cout << "Parsed JSON into glz::json_t: " << jsonString << std::endl;

    Registry loaded;
    auto error2 = glz::read_json(loaded, jsonString);
    if (error2) {
        std::cerr << "Error parsing JSON string into glz::json_t: " << error2 << std::endl;
        return 1;
    }

    std::cout << std::get<std::vector<Component<Pos>>>(loaded.components).size() << "\n";

    return 0;
}

