//
// Created by Jeppe Nielsen on 24/01/2025.
//

#pragma once
#include "IRegistrySerializer.hpp"
#include <nlohmann/json.hpp>
#include "EntitySerializer.hpp"

namespace LittleCore {

    template<typename TComponentSerializerRegistry>
    struct RegistrySerializer : public IRegistrySerializer {

        RegistrySerializer(TComponentSerializerRegistry& componentSerializerRegistry) :
        componentSerializerRegistry(componentSerializerRegistry) { }

        void Serialize(std::ostream& stream, const entt::registry& registry) const override {
            nlohmann::json json;

            const auto& storage = registry.storage<entt::entity>();
            for (auto entity : *storage) {
                componentSerializerRegistry.SerializeEntity(json,registry,entity);
            }

            stream << json.dump(4);
        }

        void Deserialize(std::istream& stream, entt::registry& registry) const override {
            nlohmann::json json;
            stream >> json;
            for (const auto &[key, value] : json.items()) {
                auto wantedEntityId = static_cast<entt::entity>(std::stoul(key));
                auto entity = registry.create(wantedEntityId);
                componentSerializerRegistry.DeserializeEntity(value,registry,entity);
            }
        }

    private:
        TComponentSerializerRegistry& componentSerializerRegistry;
    };

}
