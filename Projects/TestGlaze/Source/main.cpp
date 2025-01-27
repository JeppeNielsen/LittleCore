#include <iostream>
#include <string>
#include <vector>
#include <entt/entt.hpp>
#include <glaze/glaze.hpp>


struct Velocity {
    float dx, dy;
};

struct Position {
    float x, y;
    std::vector<Velocity> velocities;
};



// Register components with glaze
/*template <>
struct glz::meta<Position> {
    static constexpr auto value = glz::object("x", &Position::x, "y", &Position::y);
};

template <>
struct glz::meta<Velocity> {
    static constexpr auto value = glz::object("dx", &Velocity::dx, "dy", &Velocity::dy);
};
 */

struct my_struct
{
    int i = 287;
    double d = 3.14;
    std::string hello = "Hello World";
    std::array<uint64_t, 3> arr = {1, 2, 3};
};

struct Address {
    std::string street;
    std::string city;
    int zip_code;
};

int main_old() {
    // Create an entt registry
    entt::registry registry;

    // Add some entities and components
    auto entity1 = registry.create();
    registry.emplace<Position>(entity1, 1.0f, 2.0f);
    registry.emplace<Velocity>(entity1, 0.5f, 0.5f);

    auto entity2 = registry.create();
    registry.emplace<Position>(entity2, 3.0f, 4.0f);


    // Serialize the registry to JSON
    std::vector<Position> positions = {{2.0f, 3.0f}, {1.0f,4.0f}};
    positions[0].velocities.push_back({23,12});

    std::string json = glz::write_json(positions).value();

    std::cout << '\n' << json << '\n';

    std::vector<Position> newPositions;
    auto error = glz::read_json(newPositions, json);

    std::cout << error.custom_error_message << "\n";

    for(auto p : newPositions) {
        std::cout << "New position, x:" << p.x << ", y:"<<p.y<<"\n";
        for(auto v : p.velocities) {
            std::cout << "   Velocity, x:" <<v.dx << ", y:"<<v.dy<<"\n";

        }

    }

    // Create an Address object
    Address address = {"123 Main St", "Springfield", 12345};

    // Create a glz::json_t object to hold the data
    glz::json_t data;


    std::string testName = "egruiherg";

    // Add primitive data
    data["name"] = testName;
    data["age"] = 30;

    // Serialize the custom object directly by using `glz::write_json`
    std::string address_json;
    glz::write_json(address, address_json);  // Serialize the custom object into a string

    // Now add the serialized custom object as a string to the json_t object
    data["address"] = address_json;

    // Serialize the entire JSON structure to a string
    std::string json_result;
    glz::write_json(data, json_result);

    // Output the serialized JSON
    std::cout << "Serialized JSON: " << json_result << std::endl;


    return 0;
}
