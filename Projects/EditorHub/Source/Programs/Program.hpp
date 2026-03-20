//
// Created by Jeppe Nielsen on 20/03/2026.
//

#pragma once
#include <future>
#include <string>
#include "ProgramCompilerResult.hpp"
#include "ProgramDefinition.hpp"
#include "Timer.hpp"

class Program {
public:
    Program(ProgramDefinition definition, std::string workingDirectory);
    ~Program();

    void Update();

    const ProgramDefinition& Definition() const;

    void StartBuild(bool launchAfterSuccess = false);
    bool IsBuilding() const;
    bool HasBuildResult() const;
    const ProgramCompilerResult& LastBuildResult() const;
    float LastBuildDuration() const;

    void StartProcess();
    void StopProcess();
    void RestartProcess();
    bool IsProcessRunning() const;

    bool HasExitCode() const;
    int LastExitCode() const;
    const std::string& RuntimeMessage() const;

private:
    ProgramDefinition definition;
    std::string workingDirectory;

    bool isBuilding = false;
    bool launchAfterBuild = false;
    std::future<ProgramCompilerResult> buildFuture;
    LittleCore::Timer buildTimer;
    bool hasBuildResult = false;
    ProgramCompilerResult lastBuildResult;
    float lastBuildDuration = 0.0f;

    int processId = 0;
    bool isProcessRunning = false;
    bool hasExitCode = false;
    int lastExitCode = 0;
    std::string runtimeMessage;

    void PollProcess();
    void UpdateBuild();
    void SetExitStatus(int status);
};
