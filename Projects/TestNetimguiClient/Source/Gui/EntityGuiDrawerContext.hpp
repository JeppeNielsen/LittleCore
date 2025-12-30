//
// Created by Jeppe Nielsen on 14/10/2025.
//

#pragma once
#include "DefaultResourceManager.hpp"
#include "IComponentTypeNameGetter.hpp"
#include "IComponentGuiDrawer.hpp"

namespace LittleCore {

    struct EntityGuiDrawerContext {
        EntityGuiDrawerContext(DefaultResourceManager& resourceManager) : resourceManager(resourceManager) {}

        DefaultResourceManager& resourceManager;
        RegistrySerializerBase* registrySerializer;
        IComponentTypeNameGetter* componentTypeGetter;
        IComponentGuiDrawer* componentGuiDrawer;
        entt::registry* currentRegistry;
        entt::entity currentEntity;
    };

}
