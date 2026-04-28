//
// Created by Jeppe Nielsen on 20/03/2026.
//

#include "Program.hpp"
#include "LldbSession.hpp"
#include <array>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstring>
#include <filesystem>
#include <fcntl.h>
#include <poll.h>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>

namespace {
    constexpr std::size_t MaxProcessOutputSize = 128000;
}

Program::Program(ProgramDefinition definition, std::string workingDirectory) :
        definition(std::move(definition)),
        workingDirectory(std::filesystem::path(std::move(workingDirectory)).lexically_normal().generic_string()) {
}

Program::~Program() {
    StopProcess();
    CloseProcessOutputPipe();
    if (isBuilding && buildFuture.valid()) {
        buildFuture.wait();
    }
}

void Program::Update() {
    UpdateBuild();
    if (debugger) {
        debugger->Poll();
        SyncDebuggerState();
        if (!debugger->IsActive()) {
            debugger.reset();
        }
        return;
    }

    PollProcess();
}

const ProgramDefinition& Program::Definition() const {
    return definition;
}

void Program::StartBuild(bool launchAfterSuccess) {
    if (isBuilding) {
        return;
    }

    if (isProcessRunning || debugger) {
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
    if (isBuilding || isProcessRunning || debugger) {
        return;
    }

    if (!definition.ExecutableExists()) {
        runtimeMessage = "Executable not found. Compile the program first.";
        return;
    }

    const auto executablePath = definition.ExecutablePath();
    int outputPipe[2] = {-1, -1};
    if (pipe(outputPipe) != 0) {
        runtimeMessage = "Failed to create process output pipe: " + std::string(std::strerror(errno));
        return;
    }

    const pid_t pid = fork();
    if (pid < 0) {
        close(outputPipe[0]);
        close(outputPipe[1]);
        runtimeMessage = "Failed to launch process: " + std::string(std::strerror(errno));
        return;
    }

    if (pid == 0) {
        close(outputPipe[0]);
        setpgid(0, 0);
        if (!workingDirectory.empty()) {
            chdir(workingDirectory.c_str());
        }
        dup2(outputPipe[1], STDOUT_FILENO);
        dup2(outputPipe[1], STDERR_FILENO);
        close(outputPipe[1]);
        execl(executablePath.c_str(), executablePath.c_str(), static_cast<char*>(nullptr));
        _exit(127);
    }

    close(outputPipe[1]);
    setpgid(pid, pid);

    const int flags = fcntl(outputPipe[0], F_GETFL, 0);
    if (flags >= 0) {
        (void)fcntl(outputPipe[0], F_SETFL, flags | O_NONBLOCK);
    }

    processId = static_cast<int>(pid);
    CloseProcessOutputPipe();
    processOutputReadFd = outputPipe[0];
    isProcessRunning = true;
    hasExitCode = false;
    processOutput.clear();
    runtimeMessage = "Running";
}

void Program::StopProcess() {
    if (debugger) {
        debugger->KillInferiorAndQuit();
        debugger->Poll();
        SyncDebuggerState();
        return;
    }

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

    PollProcessOutput();
    CloseProcessOutputPipe();
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

void Program::SetSourceBreakpoints(const std::vector<SourceBreakpoint>& breakpoints) {
    sourceBreakpoints = breakpoints;
    if (debugger) {
        debugger->SetSourceBreakpoints(sourceBreakpoints);
    }
}

const std::vector<SourceBreakpoint>& Program::SourceBreakpoints() const {
    return sourceBreakpoints;
}

void Program::StartDebugging() {
    if (isBuilding || debugger) {
        return;
    }

    if (isProcessRunning) {
        StopProcess();
    }

    if (!definition.ExecutableExists()) {
        runtimeMessage = "Executable not found. Compile the program first.";
        return;
    }

    processOutput.clear();
    CloseProcessOutputPipe();
    debugger = std::make_unique<LldbSession>(definition.ExecutablePath(), workingDirectory);
    debugger->SetSourceBreakpoints(sourceBreakpoints);
    hasExitCode = false;
    debugger->Launch();
    SyncDebuggerState();
}

void Program::AttachDebugger() {
    if (isBuilding || debugger) {
        return;
    }

    if (!isProcessRunning || processId == 0) {
        runtimeMessage = "Start the program before attaching LLDB.";
        return;
    }

    processOutput.clear();
    CloseProcessOutputPipe();
    debugger = std::make_unique<LldbSession>(definition.ExecutablePath(), workingDirectory);
    debugger->SetSourceBreakpoints(sourceBreakpoints);
    hasExitCode = false;
    debugger->Attach(processId);
    SyncDebuggerState();
}

void Program::ContinueDebugger() {
    if (debugger) {
        debugger->Continue();
    }
}

void Program::PauseDebugger() {
    if (debugger) {
        debugger->Pause();
    }
}

void Program::StepIntoDebugger() {
    if (debugger) {
        debugger->StepInto();
    }
}

void Program::StepOverDebugger() {
    if (debugger) {
        debugger->StepOver();
    }
}

void Program::StepOutDebugger() {
    if (debugger) {
        debugger->StepOut();
    }
}

void Program::DetachDebugger() {
    if (debugger && debugger->IsAttachedToExistingProcess()) {
        debugger->DetachAndQuit();
    }
}

bool Program::IsDebuggerActive() const {
    return debugger != nullptr;
}

bool Program::IsDebuggerRunning() const {
    return debugger && debugger->IsInferiorRunning();
}

bool Program::IsDebuggerStopped() const {
    return debugger && debugger->IsInferiorStopped();
}

bool Program::IsDebuggerAttachedToProcess() const {
    return debugger && debugger->IsAttachedToExistingProcess();
}

bool Program::HasDebuggerLocation() const {
    return debugger && debugger->HasCurrentLocation();
}

const std::string& Program::DebuggerLocationFile() const {
    static const std::string empty;
    return debugger ? debugger->CurrentLocationFile() : empty;
}

int Program::DebuggerLocationLine() const {
    return debugger ? debugger->CurrentLocationLine() : 0;
}

const std::string& Program::DebuggerStatusText() const {
    static const std::string empty;
    return debugger ? debugger->StatusText() : empty;
}

const std::string& Program::DebuggerConsoleOutput() const {
    static const std::string empty;
    return debugger ? debugger->ConsoleOutput() : empty;
}

const std::vector<DebuggerScope>& Program::DebuggerScopes() const {
    static const std::vector<DebuggerScope> empty;
    return debugger ? debugger->CurrentScopes() : empty;
}

void Program::EnsureDebuggerVariableChildrenLoaded(int variablesReference) {
    if (debugger) {
        debugger->EnsureVariableChildrenLoaded(variablesReference);
    }
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

const std::string& Program::ProcessOutput() const {
    return processOutput;
}

void Program::ClearProcessOutput() {
    processOutput.clear();
}

void Program::PollProcess() {
    if (!isProcessRunning || processId == 0) {
        PollProcessOutput();
        return;
    }

    PollProcessOutput();

    int status = 0;
    const pid_t waitResult = waitpid(processId, &status, WNOHANG);
    if (waitResult == 0) {
        return;
    }

    if (waitResult == static_cast<pid_t>(processId)) {
        PollProcessOutput();
        CloseProcessOutputPipe();
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

void Program::SyncDebuggerState() {
    if (!debugger) {
        return;
    }

    if (debugger->HasInferiorProcess()) {
        processId = debugger->InferiorPid();
    } else if (!debugger->IsAttachedToExistingProcess()) {
        processId = 0;
    }

    isProcessRunning = debugger->IsInferiorRunning();
    runtimeMessage = debugger->StatusText();

    if (debugger->HasExitCode()) {
        hasExitCode = true;
        lastExitCode = debugger->ExitCode();
    }
}

void Program::PollProcessOutput() {
    if (processOutputReadFd < 0) {
        return;
    }

    std::array<char, 4096> buffer{};
    while (true) {
        pollfd pollDescriptor{};
        pollDescriptor.fd = processOutputReadFd;
        pollDescriptor.events = POLLIN | POLLHUP | POLLERR;
        const int pollResult = poll(&pollDescriptor, 1, 0);
        if (pollResult <= 0 || (pollDescriptor.revents & (POLLIN | POLLHUP | POLLERR)) == 0) {
            return;
        }

        const auto bytesRead = read(processOutputReadFd, buffer.data(), buffer.size());
        if (bytesRead > 0) {
            AppendProcessOutput(buffer.data(), static_cast<std::size_t>(bytesRead));
            continue;
        }

        if (bytesRead == 0) {
            CloseProcessOutputPipe();
            return;
        }

        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return;
        }

        CloseProcessOutputPipe();
        return;
    }
}

void Program::AppendProcessOutput(const char* text, std::size_t length) {
    processOutput.append(text, length);
    if (processOutput.size() <= MaxProcessOutputSize) {
        return;
    }

    processOutput.erase(0, processOutput.size() - MaxProcessOutputSize);
}

void Program::CloseProcessOutputPipe() {
    if (processOutputReadFd < 0) {
        return;
    }

    close(processOutputReadFd);
    processOutputReadFd = -1;
}
