//
// Created by Jeppe Nielsen on 02/12/2025.
//


#pragma once
#include <entt/entt.hpp>
#include "Label.hpp"
#include "Mesh.hpp"
#include "SystemBase.hpp"
#include "Texturable.hpp"

namespace LittleCore {
    class LabelMeshSystem : SystemBase {
    public:
        LabelMeshSystem(entt::registry& registry);
        void Update();
    private:
        entt::observer observer;
    };
}
