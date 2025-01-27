//
// Created by Jeppe Nielsen on 24/01/2025.
//

#pragma once
#include "IRegistrySerializer.hpp"
#include "EntitySerializer.hpp"

namespace LittleCore {

    template<typename TEntitySerializer>
    struct RegistrySerializer : public IRegistrySerializer {

        RegistrySerializer(TEntitySerializer& entitySerializer) :
                entitySerializer(entitySerializer) { }

        bool Serialize(std::ostream& stream, const entt::registry& registry) const override {
            glz::json_t json;

            const auto& storage = registry.storage<entt::entity>();
            for (auto entity: *storage) {
                if (!entitySerializer.SerializeEntity(json, registry, entity)) {
                    return false;
                }
            }

            std::string output;
            auto error = glz::write_json(json, output);
            if (!error) {
                stream << output;
                return true;
            }
            return false;
        }

        bool Deserialize(std::istream& stream, entt::registry& registry) const override {
            std::string jsonString;
            stream >> jsonString;
            glz::json_t json;
            auto error = glz::read_json(json, jsonString);

            if (error) {
                return false;
            }

            auto& obj = json.get_object();
            for (const auto &[key, value] : obj) {
                auto entityId = std::stoul(key);
                auto wantedEntityId = static_cast<entt::entity>(entityId);
                auto entity = registry.create(wantedEntityId);
                if (!entitySerializer.DeserializeEntity(value,registry,entity)) {
                    return false;
                }
            }

            return true;
        }

    private:
        TEntitySerializer& entitySerializer;
    };

}
