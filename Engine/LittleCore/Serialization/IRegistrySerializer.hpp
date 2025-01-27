//
// Created by Jeppe Nielsen on 24/01/2025.
//

#pragma once
#include <istream>
#include <ostream>
#include <entt/entt.hpp>

namespace LittleCore {

    struct IRegistrySerializer {

        virtual ~IRegistrySerializer() = default;

        virtual bool Serialize(std::ostream& stream, const entt::registry& registry) const = 0;
        virtual bool Deserialize(std::istream& stream, entt::registry& registry) const = 0;

    };

}