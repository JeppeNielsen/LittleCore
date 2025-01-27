 //
// Created by Jeppe Nielsen on 26/01/2025.
//

#include "RegistrySerializer.hpp"
#include "ComponentSerializer.hpp"
#include <iostream>
#include <sstream>

using namespace LittleCore;

struct Position {
    float x;
    float y;
};

struct Velocity {
    float vx;
    float vy;
};

struct Texturable {

    int texture;

};


struct TexturableSerializer : public ComponentSerializer<Texturable> {

    bool Serialize(glz::json_t& json, const Texturable& component) override {
        std::string imageFile = "<null>";
        if (component.texture == 5) {
            imageFile = "Image.png";
            json["texture"] = imageFile;
        } else {
            json["texture"] = imageFile;
        }
        return true;
    }

    bool Deserialize(const glz::json_t &json, Texturable& texturable) override {
        texturable.texture = 0;
        if (json.contains("texture")) {
            if (json["texture"].get_string() == "Image.png") {
                texturable.texture = 5;
            }
        }
        return true;
    }
};


 std::string pretty_print_json(const std::string& json) {
     std::string pretty_json;
     int indent_level = 0;
     bool in_quotes = false;

     for (size_t i = 0; i < json.size(); ++i) {
         char ch = json[i];

         switch (ch) {
             case '{':
             case '[':
                 if (!in_quotes) {
                     pretty_json += ch;
                     pretty_json += '\n';
                     indent_level++;
                     pretty_json += std::string(indent_level * 4, ' ');
                 } else {
                     pretty_json += ch;
                 }
                 break;

             case '}':
             case ']':
                 if (!in_quotes) {
                     pretty_json += '\n';
                     indent_level--;
                     pretty_json += std::string(indent_level * 4, ' ');
                     pretty_json += ch;
                 } else {
                     pretty_json += ch;
                 }
                 break;

             case ',':
                 if (!in_quotes) {
                     pretty_json += ch;
                     pretty_json += '\n';
                     pretty_json += std::string(indent_level * 4, ' ');
                 } else {
                     pretty_json += ch;
                 }
                 break;

             case ':':
                 if (!in_quotes) {
                     pretty_json += ": ";
                 } else {
                     pretty_json += ch;
                 }
                 break;

             case '"':
                 pretty_json += ch;
                 if (i > 0 && json[i - 1] != '\\') {
                     in_quotes = !in_quotes;
                 }
                 break;

             default:
                 pretty_json += ch;
                 break;
         }
     }

     return pretty_json;
 }


using DefaultEntitySerializer = EntitySerializer<Position, Velocity, TexturableSerializer>;

using DefaultRegistrySerializer = RegistrySerializer<DefaultEntitySerializer>;

int main() {

    entt::registry registry;

    auto entity1 = registry.create();
    registry.emplace<Position>(entity1, 1.0f,2.0f);
    registry.emplace<Velocity>(entity1, 3.0f,4.0f);
    registry.emplace<Texturable>(entity1, 5);

    DefaultEntitySerializer entitySerializer;
    DefaultRegistrySerializer registrySerializer(entitySerializer);
    if (false){
        std::ofstream outputFile("registry.json");
        registrySerializer.Serialize(outputFile, registry);
    }

    {
        std::ifstream inputFile("registry.json");
        entt::registry deserializedRegistry;
        if (!registrySerializer.Deserialize(inputFile, deserializedRegistry)) {
             std::cout << "Could not deserialize"<<std::endl;
        }

        for (auto [entity, pos, vel, tex]: deserializedRegistry.view<Position, Velocity, Texturable>().each()) {
            std::cout << "Position: " << pos.x << ", " << pos.y << '\n';
            std::cout << "Velocity: " << vel.vx << ", " << vel.vy << '\n';
            std::cout << "Texturable: " << tex.texture << '\n';
        }
    }

    //registrySerializer

    /*std::ofstream file("registry.json");
    registrySerializer.Serialize(file, registry);
    file.close();
    */


    return 0;
}