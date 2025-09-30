//
// Created by Jeppe Nielsen on 30/09/2025.
//

#pragma once

#include "WorldTransform.hpp"
#include "Mesh.hpp"
#include "Renderable.hpp"
#include "OctreeSystem.hpp"
#include "Ray.hpp"
#include "SystemBase.hpp"

namespace LittleCore {
    template<typename T = void>
    class PickingSystem {
    public:
        using PickingOctreeSystem = OctreeSystem<WorldTransform, Mesh, Renderable>;
        using Results = std::vector<entt::entity>;

        PickingSystem(entt::registry& registry) : pickingOctreeSystem(registry) {

        }

        Results Pick(Ray worldRay) {
            Results results;

            pickingOctreeSystem.Query(worldRay, results);

            return results;
        }

        void Update() {
            pickingOctreeSystem.Update();
        }

    private:
        PickingOctreeSystem pickingOctreeSystem;
    };
}
