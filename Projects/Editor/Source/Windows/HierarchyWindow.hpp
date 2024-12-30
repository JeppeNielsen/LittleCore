//
// Created by Jeppe Nielsen on 29/12/2024.
//


#pragma once
#include "../ScriptsInclude/EngineContext.hpp"


class HierarchyWindow {
public:
    HierarchyWindow(EngineContext& engineContext);

    void DrawGui();


private:
    EngineContext& engineContext;

};
