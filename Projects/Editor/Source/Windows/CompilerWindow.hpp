//
// Created by Jeppe Nielsen on 30/11/2024.
//


#pragma once
#include "TaskRunner.hpp"
#include "Timer.hpp"
#include "../Compilation/ProjectCompiler.hpp"
#include <functional>

class CompilerWindow {
public:

    CompilerWindow(ProjectCompiler& compiler);

    void DrawGui();

private:
    void DrawErrors();
    void Compile();
    ProjectCompiler& compiler;
    bool open;
};
