//
// Created by Jeppe Nielsen on 13/10/2025.
//
#pragma once
#include "Project.hpp"
#include "DefaultResourceManager.hpp"
#include "EditorSimulationRegistry.hpp"

namespace LittleCore {
    class EditorSimulation;

    class ProjectWindow {
    public:
        void Draw(Project& project, DefaultResourceManager& defaultResourceManager, EditorSimulationRegistry& editorSimulationRegistry);
    };
}
