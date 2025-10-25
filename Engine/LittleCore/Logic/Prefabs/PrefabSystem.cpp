//
// Created by Jeppe Nielsen on 25/10/2025.
//

#include "PrefabSystem.hpp"

using namespace LittleCore;

PrefabSystem::PrefabSystem(entt::registry& registry) : SystemBase(registry),
    observer(registry, entt::collector
            .update<PrefabInstance>().where<Hierarchy>()
            .group<PrefabInstance, Hierarchy>()) {

}

void PrefabSystem::Update() {
    if (observer.empty()) {
        return;
    }

    for(auto e : observer) {
        RefreshInstance(e);
    }
    observer.clear();
}

void PrefabSystem::RefreshInstance(entt::entity entity) {





}
