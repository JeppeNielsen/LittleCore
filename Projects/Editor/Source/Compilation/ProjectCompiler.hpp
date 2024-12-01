//
// Created by Jeppe Nielsen on 30/11/2024.
//


#pragma once
#include "TaskRunner.hpp"
#include "Timer.hpp"
#include "../Project/Project.hpp"
#include "IProjectCompilerHandler.hpp"

class ProjectCompiler {

public:

    ProjectCompiler(Project& project, IProjectCompilerHandler& handler);

    void Compile();

    bool IsCompiling() const;

    void Update();

    const CompilationResult& Result() const;

private:
    LittleCore::TaskRunner taskRunner;
    bool isCompiling = false;
    std::vector<std::string> errorsFromCompilation;
    LittleCore::Timer compilationTimer;
    Project& project;
    IProjectCompilerHandler& handler;
    CompilationResult result;
};
