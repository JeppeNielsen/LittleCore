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

    std::vector<std::string> GetComponentIds() {
        std::vector<std::string> ids;
        LittleCore::TupleHelper::for_each(editors, [&ids] (auto& editor) {
            ids.push_back(editor.GetId());
        });

        return ids;
    }

    void CreateComponent(entt::registry& registry, entt::entity entity, const std::string& componentId) {
        LittleCore::TupleHelper::for_each(editors, [&registry, entity, &componentId] (auto& editor) {
            if (componentId == editor.GetId()) {
                editor.Create(registry, entity);
            }
        });
    }

    bool DoesEntityHaveComponent(entt::registry& registry, entt::entity entity, const std::string& componentId) {
        bool didHaveComponent = false;
        LittleCore::TupleHelper::for_each(editors, [&registry, entity, &componentId, &didHaveComponent] (auto& editor) {
            if (componentId == editor.GetId()) {
                didHaveComponent = editor.HasComponent(registry, entity);
            }
        });
        return didHaveComponent;
    }

    template<typename TEditor>
    TEditor& GetEditor() {
        return std::get<TEditor>(editors);
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

