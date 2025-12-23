//
// Created by Jeppe Nielsen on 22/12/2025.
//

#pragma once
#include "DefaultResourceManager.hpp"

namespace LittleCore {

    struct SerializationContext {
        DefaultResourceManager* resourceManager = nullptr;
    };

}