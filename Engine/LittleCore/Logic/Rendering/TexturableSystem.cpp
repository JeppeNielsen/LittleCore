//
// Created by Jeppe Nielsen on 19/12/2025.
//

#include "TexturableSystem.hpp"


using namespace LittleCore;

TexturableSystem::TexturableSystem(entt::registry& registry) :
        SystemBase(registry),
        observer(registry, entt::collector
                .update<Texturable>().where<Renderable>()
                .group<Texturable, Renderable>()) {

    registry.on_destroy<Texturable>().connect<&TexturableSystem::EntityDestroyed>(this);
}


void TexturableSystem::Update() {
    for(auto entity : observer) {
        auto& texturable = registry.get<Texturable>(entity);
        auto& renderable = registry.get<Renderable>(entity);
        renderable.uniforms.Set("colorTexture", texturable.operator->()->texture);
    }
    observer.clear();
}

void TexturableSystem::EntityDestroyed(entt::registry& reg, entt::entity entity) {
    auto* renderable = registry.try_get<Renderable>(entity);
    if (!renderable) {
        return;
    }
    renderable->uniforms.Remove("colorTexture");
}

void TexturableSystem::Reload() {
    auto view = registry.view<Texturable, Renderable>();

    for(auto[entity, texturable, renderable] : view.each()) {
        renderable.uniforms.Set("colorTexture", texturable.operator->()->texture);
    }

}
