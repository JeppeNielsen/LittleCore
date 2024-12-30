//
// Created by Jeppe Nielsen on 22/12/2024.
//


#pragma once
#include "../ComponentEditor.hpp"
#include "LocalBoundingBox.hpp"

class LocalBoundingBoxEditor : public ComponentEditor<LittleCore::LocalBoundingBox> {
protected:
    bool Draw(entt::registry& registry, entt::entity entity, LittleCore::LocalBoundingBox& component) override;
};
