//
// Created by Jeppe Nielsen on 19/09/2025.
//


#include "EntityGuiDrawer.hpp"
#include <iostream>
#include "StorageFactory.hpp"

using namespace LittleCore;


void EntityGuiDrawer::Draw(entt::registry& registry, entt::entity entity) {

    FactoryContext context;
    context.kind = 23;

    for(auto it : registry.storage()) {
        if (it.second.contains(entity)) {
            context.name = it.second.type().name();
            void* ptr = it.second.CreateFactory(&context);

            LittleCore::IComponentGuiDrawer* componentGuiDrawer = reinterpret_cast<LittleCore::IComponentGuiDrawer*>(ptr);
            componentGuiDrawer->Draw(registry, entity);
            delete componentGuiDrawer;
        }
    }

}
