//
// Created by Jeppe Nielsen on 22/12/2024.
//


#pragma once
#include "../ComponentEditor.hpp"
#include "Texturable.hpp"
#include "DefaultResourceManager.hpp"

class TexturableEditor : public ComponentEditor<LittleCore::Texturable> {
public:
    void SetPathMapper(LittleCore::DefaultResourceManager& resourceManager);
protected:
    bool Draw(entt::registry& registry, entt::entity entity, LittleCore::Texturable& component) override;
private:
    LittleCore::DefaultResourceManager* resourceManager;
};
