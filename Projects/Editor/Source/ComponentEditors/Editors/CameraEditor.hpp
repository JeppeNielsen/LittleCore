//
// Created by Jeppe Nielsen on 22/12/2024.
//


#pragma once
#include "../ComponentEditor.hpp"
#include "Camera.hpp"

class CameraEditor : public ComponentEditor<LittleCore::Camera> {
protected:
    bool Draw(entt::registry& registry, entt::entity entity, LittleCore::Camera& component) override;
};
