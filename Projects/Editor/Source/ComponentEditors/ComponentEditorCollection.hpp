//
// Created by Jeppe Nielsen on 22/12/2024.
//


#pragma once
#include <tuple>
#include <entt/entt.hpp>
#include "TupleHelper.hpp"

template<typename ...T>
class ComponentEditorCollection {
public:

    ComponentEditorCollection(entt::registry& registry) : registry(registry) { }

    void Draw(entt::entity entity) {
        LittleCore::TupleHelper::for_each(editors, [this, entity] (auto& editor) {
            editor.TryDraw(registry, entity);
        });
    }

private:
    std::tuple<T...> editors;
    entt::registry& registry;
};
