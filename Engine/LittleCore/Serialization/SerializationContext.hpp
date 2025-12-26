//
// Created by Jeppe Nielsen on 22/12/2025.
//

#pragma once
#include "DefaultResourceManager.hpp"
#include <entt/entt.hpp>

namespace LittleCore {

    class RegistrySerializerBase;

    struct SerializationContext {
        entt::registry* registry;
        RegistrySerializerBase* registrySerializer;
        DefaultResourceManager* resourceManager = nullptr;
    };

}