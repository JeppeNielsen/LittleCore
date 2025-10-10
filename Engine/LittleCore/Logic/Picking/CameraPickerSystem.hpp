//
// Created by Jeppe Nielsen on 09/10/2025.
//


#pragma once
#include "SystemBase.hpp"
#include "PickingSystem.hpp"

namespace LittleCore {
    class CameraPickerSystem : public SystemBase {
    public:
        CameraPickerSystem(entt::registry& registry,
                           PickingSystem<>& pickingSystem);
        void Update();
    private:
        PickingSystem<>& pickingSystem;
    };
}
