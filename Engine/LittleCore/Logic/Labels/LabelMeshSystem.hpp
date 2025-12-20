//
// Created by Jeppe Nielsen on 02/12/2025.
//


#pragma once
#include <entt/entt.hpp>
#include "Label.hpp"
#include "Mesh.hpp"
#include "SystemBase.hpp"

namespace LittleCore {
    class LabelMeshSystem : SystemBase {
    public:
        LabelMeshSystem(entt::registry& registry);
        void Update();
        void Reload();
    private:
        entt::observer observer;
    };
}
