//
// Created by Jeppe Nielsen on 14/01/2024.
//

#pragma once
#include "EngineContext.hpp"


class EngineContextFactory {
public:
    EngineContextFactory(ImGuiContext* imGuiContext);

    EngineContext Create();
private:
    ImGuiContext* imGuiContext;
};
