//
// Created by Jeppe Nielsen on 17/01/2025.
//


#pragma once


#include <ResourcePathMapper.hpp>
#include "../Project/Project.hpp"

class ProjectWindow {
public:

    ProjectWindow(Project& project);

    void DrawGui();

private:
    Project& project;

};
