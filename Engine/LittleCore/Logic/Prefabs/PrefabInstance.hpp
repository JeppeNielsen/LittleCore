//
// Created by Jeppe Nielsen on 25/10/2025.
//


#pragma once
#include "ResourceHandle.hpp"
#include "PrefabResource.hpp"

namespace LittleCore {
    class PrefabInstance {
        ResourceHandle<PrefabResource> Prefab;
        entt::entity root;
    };
}
