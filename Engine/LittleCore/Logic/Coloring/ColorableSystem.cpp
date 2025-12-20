//
// Created by Jeppe Nielsen on 20/12/2025.
//

#include "ColorableSystem.hpp"

using namespace LittleCore;

ColorableSystem::ColorableSystem(entt::registry& registry) :
        SystemBase(registry),
        observer(registry, entt::collector
                .update<Colorable>().where<Colorable>()
                .group<Colorable, Renderable>()) {

    registry.on_destroy<Colorable>().connect<&ColorableSystem::EntityDestroyed>(this);
}


void ColorableSystem::Update() {

    for(auto entity : observer) {
        const auto& colorable = registry.get<Colorable>(entity);
        auto& renderable = registry.get<Renderable>(entity);
        renderable.uniforms.Set("color", colorable.color);
    }
    observer.clear();
}

void ColorableSystem::EntityDestroyed(entt::registry& reg, entt::entity entity) {
    auto* renderable = registry.try_get<Renderable>(entity);
    if (!renderable) {
        return;
    }
    renderable->uniforms.Remove("color");
}

