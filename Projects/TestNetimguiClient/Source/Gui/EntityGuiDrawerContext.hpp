//
// Created by Jeppe Nielsen on 14/10/2025.
//

#pragma once
#include "DefaultResourceManager.hpp"

namespace LittleCore {

    struct EntityGuiDrawerContext {
        EntityGuiDrawerContext(DefaultResourceManager& resourceManager) : resourceManager(resourceManager) {}

        DefaultResourceManager& resourceManager;
    };

}
