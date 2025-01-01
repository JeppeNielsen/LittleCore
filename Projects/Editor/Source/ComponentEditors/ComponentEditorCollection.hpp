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

    void Draw(entt::registry& registry, entt::entity entity) {
        LittleCore::TupleHelper::for_each(editors, [&registry, entity] (auto& editor) {
            editor.TryDraw(registry, entity);
        });
    }

private:
    std::tuple<T...> editors;
};

template<typename...>
struct ConcatComponentEditorCollection_impl;

template<typename... s1, typename... s2>
struct ConcatComponentEditorCollection_impl<ComponentEditorCollection<s1...>, ComponentEditorCollection<s2...>> {
using type = ComponentEditorCollection<s1..., s2...>;
};

template<typename A1, typename A2>
using ConcatComponentEditorCollections = typename ConcatComponentEditorCollection_impl<A1,A2>::type;

