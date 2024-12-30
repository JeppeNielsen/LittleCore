//
// Created by Jeppe Nielsen on 22/12/2024.
//


#pragma once
#include "../ComponentEditor.hpp"
#include "LocalTransform.hpp"

class LocalTransformEditor : public ComponentEditor<LittleCore::LocalTransform> {
protected:
    bool Draw(entt::registry& registry, entt::entity entity, LittleCore::LocalTransform& component) override;
};
