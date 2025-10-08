//
// Created by Jeppe Nielsen on 05/10/2025.
//


#pragma once
#include "entt/entt.hpp"

namespace LittleCore {
    class EditorSelection {
    public:
        using Selection = std::vector<entt::entity>;

        bool IsSelected(entt::entity entity);
        void Select(entt::entity entity);
        void Deselect(entt::entity entity);
        void Toggle(entt::entity entity);
        void Clear();

        Selection selection;
    };

}
