//
// Created by Jeppe Nielsen on 05/10/2025.
//

#include "EditorSelection.hpp"

using namespace LittleCore;

bool EditorSelection::IsSelected(entt::entity entity) {
    return std::find(selection.begin(), selection.end(),entity)!=selection.end();
}

void EditorSelection::Select(entt::entity entity) {
    if (IsSelected(entity)) {
        return;
    }
    selection.push_back(entity);
}

void EditorSelection::Deselect(entt::entity entity) {
    auto it = std::find(selection.begin(), selection.end(),entity);
    if (it == selection.end()) {
        return;
    }
    selection.erase(it);
}

void EditorSelection::Toggle(entt::entity entity) {
    if (IsSelected(entity)) {
        Deselect(entity);
    } else {
        Select(entity);
    }
}

void EditorSelection::Clear() {
    selection.clear();
}


