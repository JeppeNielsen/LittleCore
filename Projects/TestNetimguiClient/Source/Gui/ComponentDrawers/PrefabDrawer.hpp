//
// Created by Jeppe Nielsen on 04/11/2025.
//

#pragma once
#include "Prefab.hpp"
#include "ObjectGuiDrawer.hpp"
#include "GuiResourceDrawers.hpp"

namespace LittleCore {
    template<>
    bool ObjectGuiDrawer<Prefab>::Draw(EntityGuiDrawerContext& context,
                                       Prefab& object) {

        GuiHelper::DrawOptions drawOptions(context.resourceManager, false, false);
        GuiHelper::Draw(drawOptions, "Prefab", object.resource);

        for(auto& serializedPrefabComponent : object.components ) {

            context.componentGuiDrawer->DrawComponent(serializedPrefabComponent.componentId, *context.currentRegistry,
                                                      serializedPrefabComponent.entity);
        }


        return drawOptions.didChange;
    };

}