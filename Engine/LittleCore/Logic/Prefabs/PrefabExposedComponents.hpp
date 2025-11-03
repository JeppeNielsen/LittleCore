//
// Created by Jeppe Nielsen on 30/10/2025.
//

#pragma once
#include <vector>
#include <string>

namespace LittleCore {
    struct PrefabExposedComponents {
        using ExposedComponents = std::vector<std::string>;
        ExposedComponents exposedComponents;
    };
}