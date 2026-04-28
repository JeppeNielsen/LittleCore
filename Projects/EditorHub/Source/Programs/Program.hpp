//
// Created by Jeppe Nielsen on 20/03/2026.
//

#pragma once
#include <cstddef>
#include <future>
#include <memory>
#include <string>
#include <vector>
#include "ProgramCompilerResult.hpp"
#include "ProgramDefinition.hpp"
#include "DebuggerVariables.hpp"
#include "SourceBreakpoint.hpp"
#include "Timer.hpp"

class LldbSession;

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

    void SetSourceBreakpoints(const std::vector<SourceBreakpoint>& breakpoints);
    const std::vector<SourceBreakpoint>& SourceBreakpoints() const;

    void StartDebugging();
    void AttachDebugger();
    void ContinueDebugger();
    void PauseDebugger();
    void StepIntoDebugger();
    void StepOverDebugger();
    void StepOutDebugger();
    void DetachDebugger();
    bool IsDebuggerActive() const;
    bool IsDebuggerRunning() const;
    bool IsDebuggerStopped() const;
    bool IsDebuggerAttachedToProcess() const;
    bool HasDebuggerLocation() const;
    const std::string& DebuggerLocationFile() const;
    int DebuggerLocationLine() const;
    const std::string& DebuggerStatusText() const;
    const std::string& DebuggerConsoleOutput() const;
    const std::vector<DebuggerScope>& DebuggerScopes() const;
    void EnsureDebuggerVariableChildrenLoaded(int variablesReference);

    bool HasExitCode() const;
    int LastExitCode() const;
    const std::string& RuntimeMessage() const;
    const std::string& ProcessOutput() const;
    void ClearProcessOutput();

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
    int processOutputReadFd = -1;
    bool isProcessRunning = false;
    bool hasExitCode = false;
    int lastExitCode = 0;
    std::string runtimeMessage;
    std::string processOutput;
    std::vector<SourceBreakpoint> sourceBreakpoints;
    std::unique_ptr<LldbSession> debugger;

    void PollProcess();
    void PollProcessOutput();
    void UpdateBuild();
    void SetExitStatus(int status);
    void SyncDebuggerState();
    void AppendProcessOutput(const char* text, std::size_t length);
    void CloseProcessOutputPipe();
};
