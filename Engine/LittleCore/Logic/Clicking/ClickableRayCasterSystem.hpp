//
// Created by Jeppe Nielsen on 31/05/2026.
//


#pragma once
#include "SystemBase.hpp"
#include "ClickableOctreeSystem.hpp"
#include "Camera.hpp"

namespace LittleCore {
    class ClickableRayCasterSystem : SystemBase {
    public:
        ClickableRayCasterSystem(entt::registry& registry);
        void Update();
    private:
        bool TryGetClosestEntityFromRay(Ray ray, entt::entity& closest, const Camera& camera, const WorldTransform& worldTransform);
        ClickableOctreeSystem clickableOctreeSystem;
    };
}
