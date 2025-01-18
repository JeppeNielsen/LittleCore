//
// Created by Jeppe Nielsen on 17/01/2025.
//


#pragma once


#include <ResourcePathMapper.hpp>

class ProjectWindow {
public:

    ProjectWindow(LittleCore::ResourcePathMapper& resourcePathMapper);

    void DrawGui();

private:
    LittleCore::ResourcePathMapper& resourcePathMapper;

};
