//
// Created by Jeppe Nielsen on 06/10/2025.
//

#pragma once
#include "entt/entt.hpp"

namespace LittleCore {

    class EditorSimulation;

    class InspectorWindow {
    public:

        void Draw(EditorSimulation& simulation);
        void DrawEntity(EditorSimulation& simulation, entt::entity entity);

    };
}