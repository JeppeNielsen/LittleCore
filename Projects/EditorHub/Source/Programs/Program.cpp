//
// Created by Jeppe Nielsen on 20/03/2026.
//

#include "Program.hpp"
#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstring>
#include <filesystem>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>

Program::Program(ProgramDefinition definition, std::string workingDirectory) :
        definition(std::move(definition)),
        workingDirectory(std::filesystem::path(std::move(workingDirectory)).lexically_normal().generic_string()) {
}

Program::~Program() {
    StopProcess();
    if (isBuilding && buildFuture.valid()) {
        buildFuture.wait();
    }
}

void Program::Update() {
    UpdateBuild();
    PollProcess();
}

const ProgramDefinition& Program::Definition() const {
    return definition;
}

void Program::StartBuild(bool launchAfterSuccess) {
    if (isBuilding) {
        return;
    }

    if (isProcessRunning) {
        StopProcess();
    }

    launchAfterBuild = launchAfterSuccess;
    isBuilding = true;
    buildTimer.Start();

    ProgramDefinition definitionCopy = definition;
    buildFuture = std::async(std::launch::async, [definitionCopy]() mutable {
        return definitionCopy.Build();
    });
}

bool Program::IsBuilding() const {
    return isBuilding;
}

bool Program::HasBuildResult() const {
    return hasBuildResult;
}

const ProgramCompilerResult& Program::LastBuildResult() const {
    return lastBuildResult;
}

float Program::LastBuildDuration() const {
    return lastBuildDuration;
}

void Program::StartProcess() {
    if (isBuilding || isProcessRunning) {
        return;
    }

    if (!definition.ExecutableExists()) {
        runtimeMessage = "Executable not found. Compile the program first.";
        return;
    }

    const auto executablePath = definition.ExecutablePath();

    const pid_t pid = fork();
    if (pid < 0) {
        runtimeMessage = "Failed to launch process: " + std::string(std::strerror(errno));
        return;
    }

    if (pid == 0) {
        setpgid(0, 0);
        if (!workingDirectory.empty()) {
            chdir(workingDirectory.c_str());
        }
        execl(executablePath.c_str(), executablePath.c_str(), static_cast<char*>(nullptr));
        _exit(127);
    }

    setpgid(pid, pid);

    processId = static_cast<int>(pid);
    isProcessRunning = true;
    hasExitCode = false;
    runtimeMessage = "Running";
}

void Program::StopProcess() {
    if (!isProcessRunning || processId == 0) {
        return;
    }

    kill(-processId, SIGTERM);

    int status = 0;
    bool didExit = false;
    for (int i = 0; i < 30; ++i) {
        const pid_t waitResult = waitpid(processId, &status, WNOHANG);
        if (waitResult == static_cast<pid_t>(processId)) {
            didExit = true;
            break;
        }
        usleep(10000);
    }

    if (!didExit) {
        kill(-processId, SIGKILL);
        waitpid(processId, &status, 0);
    }

    isProcessRunning = false;
    processId = 0;
    SetExitStatus(status);
    runtimeMessage = "Stopped";
}

void Program::RestartProcess() {
    StopProcess();
    StartProcess();
}

bool Program::IsProcessRunning() const {
    return isProcessRunning;
}

bool Program::HasExitCode() const {
    return hasExitCode;
}

int Program::LastExitCode() const {
    return lastExitCode;
}

const std::string& Program::RuntimeMessage() const {
    return runtimeMessage;
}

void Program::PollProcess() {
    if (!isProcessRunning || processId == 0) {
        return;
    }

    int status = 0;
    const pid_t waitResult = waitpid(processId, &status, WNOHANG);
    if (waitResult == 0) {
        return;
    }

    if (waitResult == static_cast<pid_t>(processId)) {
        isProcessRunning = false;
        processId = 0;
        SetExitStatus(status);
    }
}

void Program::UpdateBuild() {
    if (!isBuilding || !buildFuture.valid()) {
        return;
    }

    if (buildFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        return;
    }

    lastBuildResult = buildFuture.get();
    lastBuildDuration = buildTimer.Stop();
    hasBuildResult = true;
    isBuilding = false;

    std::ostringstream summary;
    summary << lastBuildResult.summary << "\nBuild time: " << lastBuildDuration << " seconds";
    lastBuildResult.summary = summary.str();

    if (launchAfterBuild && lastBuildResult.succeeded) {
        StartProcess();
    }

    launchAfterBuild = false;
}

void Program::SetExitStatus(int status) {
    hasExitCode = true;

    if (WIFEXITED(status)) {
        lastExitCode = WEXITSTATUS(status);
        runtimeMessage = lastExitCode == 0
                         ? "Exited normally"
                         : "Exited with code " + std::to_string(lastExitCode);
        return;
    }

    if (WIFSIGNALED(status)) {
        lastExitCode = 128 + WTERMSIG(status);
        runtimeMessage = "Terminated by signal " + std::to_string(WTERMSIG(status));
        return;
    }

    lastExitCode = status;
    runtimeMessage = "Process stopped";
}
