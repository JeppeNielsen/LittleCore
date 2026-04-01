//
// Created by Codex on 01/04/2026.
//

#pragma once
#include "DebuggerVariables.hpp"
#include "SourceBreakpoint.hpp"
#include <string>
#include <vector>

class LldbSession {
public:
    LldbSession(std::string executablePath, std::string workingDirectory);
    ~LldbSession();

    LldbSession(const LldbSession&) = delete;
    LldbSession& operator=(const LldbSession&) = delete;

    bool Start();
    void Poll();

    void SetSourceBreakpoints(const std::vector<SourceBreakpoint>& breakpoints);
    void Launch();
    void Attach(int pid);
    void Continue();
    void Pause();
    void StepInto();
    void StepOver();
    void StepOut();
    void DetachAndQuit();
    void KillInferiorAndQuit();

    bool IsActive() const;
    bool HasInferiorProcess() const;
    int InferiorPid() const;
    bool IsInferiorRunning() const;
    bool IsInferiorStopped() const;
    bool IsAttachedToExistingProcess() const;
    bool HasCurrentLocation() const;
    const std::string& CurrentLocationFile() const;
    int CurrentLocationLine() const;
    bool HasExitCode() const;
    int ExitCode() const;
    const std::string& StatusText() const;
    const std::string& ConsoleOutput() const;
    const std::vector<DebuggerScope>& CurrentScopes() const;

private:
    enum class RequestedAction {
        None,
        Launch,
        Attach,
    };

    enum class CommandKind {
        Generic,
        Initialize,
        Launch,
        Attach,
        Continue,
        Pause,
        StepInto,
        StepOver,
        StepOut,
        Threads,
        StackTrace,
        Scopes,
        Variables,
        SetBreakpoints,
        ConfigurationDone,
        Disconnect,
    };

    struct PendingCommand {
        CommandKind kind = CommandKind::Generic;
        std::string description;
        int referenceId = 0;
        int stopGeneration = 0;
    };

    std::string executablePath;
    std::string workingDirectory;
    std::vector<SourceBreakpoint> sourceBreakpoints;
    int dapInputFd = -1;
    int dapOutputFd = -1;
    int dapErrorFd = -1;
    int dapPid = 0;
    int inferiorPid = 0;
    int nextRequestSeq = 1;
    int activeThreadId = 0;
    int currentFrameId = 0;
    int currentStopGeneration = 0;
    RequestedAction requestedAction = RequestedAction::None;
    int requestedAttachPid = 0;
    bool initializeSucceeded = false;
    bool receivedInitializedEvent = false;
    bool configurationDoneSent = false;
    bool inferiorRunning = false;
    bool inferiorStopped = false;
    bool attachedToExistingProcess = false;
    bool hasCurrentLocation = false;
    bool hasExitCode = false;
    bool keepInferiorAliveOnExit = false;
    bool disconnectRequested = false;
    bool shutdownAfterStartupFailure = false;
    int exitCode = 0;
    int currentLocationLine = 0;
    std::string currentLocationFile;
    std::string statusText;
    std::string consoleOutput;
    std::string readBuffer;
    std::string pendingStopReason;
    std::vector<std::pair<int, PendingCommand>> pendingCommands;
    std::vector<std::string> knownBreakpointFiles;
    std::vector<DebuggerScope> currentScopes;

    void CloseSession(bool terminateProcess);
    void ClearCurrentFrameData();
    void BeginRequestedSession();
    void RefreshBreakpoints();
    void RequestThreads();
    void RequestStackTrace(int threadId);
    void RequestScopes(int frameId);
    void RequestVariables(int variablesReference);
    void QueueDisconnect(bool terminateDebuggee, const std::string& description);
    void SendRequest(const std::string& command,
                     const std::string& argumentsJson,
                     CommandKind kind,
                     std::string description,
                     int referenceId = 0,
                     int stopGeneration = -1);
    void HandleMessage(const std::string& message);
    void HandleEvent(const std::string& message);
    void HandleResponse(const std::string& message);
    void UpdateStopStatus();
    void AppendConsoleOutput(const std::string& text);

    static std::string EscapeJsonString(const std::string& value);
    static std::string ResolveLldbDapPath();
    static std::string ResolveDebugserverPath();
};
