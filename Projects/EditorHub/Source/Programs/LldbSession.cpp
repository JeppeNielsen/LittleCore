//
// Created by Codex on 01/04/2026.
//

#include "LldbSession.hpp"
#include "FileHelper.hpp"
#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cctype>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <optional>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>

namespace {
    constexpr std::size_t MaxConsoleOutputSize = 64000;

    bool StartsWith(const std::string& value, const std::string& prefix) {
        return value.size() >= prefix.size() && value.compare(0, prefix.size(), prefix) == 0;
    }

    std::size_t SkipWhitespace(const std::string& text, std::size_t index) {
        while (index < text.size() && std::isspace(static_cast<unsigned char>(text[index])) != 0) {
            ++index;
        }
        return index;
    }

    std::optional<std::pair<std::string, std::size_t>> ParseJsonStringToken(const std::string& text, std::size_t quoteIndex) {
        if (quoteIndex >= text.size() || text[quoteIndex] != '"') {
            return std::nullopt;
        }

        std::string value;
        bool escaped = false;
        for (std::size_t i = quoteIndex + 1; i < text.size(); ++i) {
            const char c = text[i];
            if (escaped) {
                switch (c) {
                    case '"':
                    case '\\':
                    case '/':
                        value += c;
                        break;
                    case 'b':
                        value += '\b';
                        break;
                    case 'f':
                        value += '\f';
                        break;
                    case 'n':
                        value += '\n';
                        break;
                    case 'r':
                        value += '\r';
                        break;
                    case 't':
                        value += '\t';
                        break;
                    default:
                        value += c;
                        break;
                }
                escaped = false;
                continue;
            }

            if (c == '\\') {
                escaped = true;
                continue;
            }

            if (c == '"') {
                return std::make_pair(std::move(value), i);
            }

            value += c;
        }

        return std::nullopt;
    }

    std::optional<std::size_t> FindJsonValue(const std::string& text, const std::string& key, std::size_t searchStart = 0) {
        const std::string pattern = "\"" + key + "\"";
        auto keyPos = text.find(pattern, searchStart);
        while (keyPos != std::string::npos) {
            auto cursor = SkipWhitespace(text, keyPos + pattern.size());
            if (cursor < text.size() && text[cursor] == ':') {
                return SkipWhitespace(text, cursor + 1);
            }
            keyPos = text.find(pattern, keyPos + pattern.size());
        }
        return std::nullopt;
    }

    std::optional<std::size_t> FindTopLevelJsonValue(const std::string& text, const std::string& key) {
        bool inString = false;
        bool escaped = false;
        int objectDepth = 0;
        int arrayDepth = 0;

        for (std::size_t i = 0; i < text.size(); ++i) {
            const char c = text[i];
            if (inString) {
                if (escaped) {
                    escaped = false;
                } else if (c == '\\') {
                    escaped = true;
                } else if (c == '"') {
                    inString = false;
                }
                continue;
            }

            switch (c) {
                case '"': {
                    if (objectDepth == 1 && arrayDepth == 0) {
                        const auto token = ParseJsonStringToken(text, i);
                        if (!token.has_value()) {
                            return std::nullopt;
                        }

                        const auto cursor = SkipWhitespace(text, token->second + 1);
                        if (cursor < text.size() && text[cursor] == ':' && token->first == key) {
                            return SkipWhitespace(text, cursor + 1);
                        }

                        i = token->second;
                    } else {
                        inString = true;
                    }
                    break;
                }
                case '{':
                    ++objectDepth;
                    break;
                case '}':
                    --objectDepth;
                    break;
                case '[':
                    ++arrayDepth;
                    break;
                case ']':
                    --arrayDepth;
                    break;
                default:
                    break;
            }
        }

        return std::nullopt;
    }

    std::optional<std::string> ExtractJsonString(const std::string& text, const std::string& key, std::size_t searchStart = 0) {
        const auto valuePos = FindJsonValue(text, key, searchStart);
        if (!valuePos.has_value()) {
            return std::nullopt;
        }

        const auto token = ParseJsonStringToken(text, *valuePos);
        return token.has_value() ? std::optional<std::string>(token->first) : std::nullopt;
    }

    std::optional<std::string> ExtractTopLevelJsonString(const std::string& text, const std::string& key) {
        const auto valuePos = FindTopLevelJsonValue(text, key);
        if (!valuePos.has_value()) {
            return std::nullopt;
        }

        const auto token = ParseJsonStringToken(text, *valuePos);
        return token.has_value() ? std::optional<std::string>(token->first) : std::nullopt;
    }

    std::optional<int> ExtractJsonInt(const std::string& text, const std::string& key, std::size_t searchStart = 0) {
        const auto valuePos = FindJsonValue(text, key, searchStart);
        if (!valuePos.has_value() || *valuePos >= text.size()) {
            return std::nullopt;
        }

        std::size_t index = *valuePos;
        if (text[index] != '-' && std::isdigit(static_cast<unsigned char>(text[index])) == 0) {
            return std::nullopt;
        }

        std::size_t end = index + 1;
        while (end < text.size() && std::isdigit(static_cast<unsigned char>(text[end])) != 0) {
            ++end;
        }

        try {
            return std::stoi(text.substr(index, end - index));
        } catch (...) {
            return std::nullopt;
        }
    }

    std::optional<bool> ExtractJsonBool(const std::string& text, const std::string& key, std::size_t searchStart = 0) {
        const auto valuePos = FindJsonValue(text, key, searchStart);
        if (!valuePos.has_value()) {
            return std::nullopt;
        }

        if (StartsWith(text.substr(*valuePos), "true")) {
            return true;
        }

        if (StartsWith(text.substr(*valuePos), "false")) {
            return false;
        }

        return std::nullopt;
    }

    std::optional<int> ExtractTopLevelJsonInt(const std::string& text, const std::string& key) {
        const auto valuePos = FindTopLevelJsonValue(text, key);
        if (!valuePos.has_value()) {
            return std::nullopt;
        }

        std::size_t index = *valuePos;
        if (index >= text.size() || (text[index] != '-' && std::isdigit(static_cast<unsigned char>(text[index])) == 0)) {
            return std::nullopt;
        }

        std::size_t end = index + 1;
        while (end < text.size() && std::isdigit(static_cast<unsigned char>(text[end])) != 0) {
            ++end;
        }

        try {
            return std::stoi(text.substr(index, end - index));
        } catch (...) {
            return std::nullopt;
        }
    }

    std::optional<bool> ExtractTopLevelJsonBool(const std::string& text, const std::string& key) {
        const auto valuePos = FindTopLevelJsonValue(text, key);
        if (!valuePos.has_value()) {
            return std::nullopt;
        }

        if (StartsWith(text.substr(*valuePos), "true")) {
            return true;
        }

        if (StartsWith(text.substr(*valuePos), "false")) {
            return false;
        }

        return std::nullopt;
    }

    std::string ToLowerAscii(std::string value) {
        for (char& c : value) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        return value;
    }

    bool ScopeShouldLoad(const std::string& name, bool expensive) {
        const auto lowered = ToLowerAscii(name);
        if (lowered.find("register") != std::string::npos) {
            return false;
        }

        if (lowered.find("local") != std::string::npos || lowered.find("argument") != std::string::npos) {
            return true;
        }

        return !expensive;
    }

    std::optional<std::pair<std::size_t, std::size_t>> FindJsonContainerBounds(const std::string& text,
                                                                                std::size_t start,
                                                                                char openChar,
                                                                                char closeChar) {
        if (start >= text.size() || text[start] != openChar) {
            return std::nullopt;
        }

        bool inString = false;
        bool escaped = false;
        int depth = 0;
        for (std::size_t i = start; i < text.size(); ++i) {
            const char c = text[i];
            if (inString) {
                if (escaped) {
                    escaped = false;
                } else if (c == '\\') {
                    escaped = true;
                } else if (c == '"') {
                    inString = false;
                }
                continue;
            }

            if (c == '"') {
                inString = true;
                continue;
            }

            if (c == openChar) {
                ++depth;
                continue;
            }

            if (c == closeChar) {
                --depth;
                if (depth == 0) {
                    return std::make_pair(start, i);
                }
            }
        }

        return std::nullopt;
    }

    std::vector<std::string> ExtractTopLevelObjectArray(const std::string& text, const std::string& key, std::size_t searchStart = 0) {
        const auto valuePos = FindJsonValue(text, key, searchStart);
        if (!valuePos.has_value() || *valuePos >= text.size() || text[*valuePos] != '[') {
            return {};
        }

        const auto bounds = FindJsonContainerBounds(text, *valuePos, '[', ']');
        if (!bounds.has_value()) {
            return {};
        }

        std::vector<std::string> objects;
        bool inString = false;
        bool escaped = false;
        int objectDepth = 0;
        int arrayDepth = 0;
        std::size_t objectStart = std::string::npos;

        for (std::size_t i = bounds->first + 1; i < bounds->second; ++i) {
            const char c = text[i];
            if (inString) {
                if (escaped) {
                    escaped = false;
                } else if (c == '\\') {
                    escaped = true;
                } else if (c == '"') {
                    inString = false;
                }
                continue;
            }

            switch (c) {
                case '"':
                    inString = true;
                    break;
                case '[':
                    ++arrayDepth;
                    break;
                case ']':
                    if (arrayDepth > 0) {
                        --arrayDepth;
                    }
                    break;
                case '{':
                    if (objectDepth == 0 && arrayDepth == 0) {
                        objectStart = i;
                    }
                    ++objectDepth;
                    break;
                case '}':
                    if (objectDepth > 0) {
                        --objectDepth;
                        if (objectDepth == 0 && arrayDepth == 0 && objectStart != std::string::npos) {
                            objects.push_back(text.substr(objectStart, i - objectStart + 1));
                            objectStart = std::string::npos;
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        return objects;
    }

    DebuggerVariable* FindVariableByReference(std::vector<DebuggerVariable>& variables, int variablesReference) {
        for (auto& variable : variables) {
            if (variable.variablesReference == variablesReference) {
                return &variable;
            }

            if (auto* child = FindVariableByReference(variable.variables, variablesReference)) {
                return child;
            }
        }

        return nullptr;
    }

    DebuggerVariable* FindVariableByReference(std::vector<DebuggerScope>& scopes, int variablesReference) {
        for (auto& scope : scopes) {
            if (auto* variable = FindVariableByReference(scope.variables, variablesReference)) {
                return variable;
            }
        }

        return nullptr;
    }

    DebuggerScope* FindScopeByReference(std::vector<DebuggerScope>& scopes, int variablesReference) {
        auto scopeIt = std::find_if(scopes.begin(), scopes.end(), [variablesReference](const DebuggerScope& scope) {
            return scope.variablesReference == variablesReference;
        });
        return scopeIt != scopes.end() ? &(*scopeIt) : nullptr;
    }

    bool BeginVariablesLoad(std::vector<DebuggerScope>& scopes, int variablesReference) {
        if (auto* scope = FindScopeByReference(scopes, variablesReference)) {
            if (scope->variablesLoaded || scope->variablesLoading) {
                return false;
            }

            scope->variablesLoading = true;
            return true;
        }

        if (auto* variable = FindVariableByReference(scopes, variablesReference)) {
            if (variable->variablesLoaded || variable->variablesLoading) {
                return false;
            }

            variable->variablesLoading = true;
            return true;
        }

        return false;
    }

    bool FinishVariablesLoad(std::vector<DebuggerScope>& scopes,
                             int variablesReference,
                             std::vector<DebuggerVariable> variables) {
        if (auto* scope = FindScopeByReference(scopes, variablesReference)) {
            scope->variables = std::move(variables);
            scope->variablesLoading = false;
            scope->variablesLoaded = true;
            return true;
        }

        if (auto* variable = FindVariableByReference(scopes, variablesReference)) {
            variable->variables = std::move(variables);
            variable->variablesLoading = false;
            variable->variablesLoaded = true;
            return true;
        }

        return false;
    }

    std::vector<DebuggerVariable> ParseDebuggerVariables(const std::string& message) {
        std::vector<DebuggerVariable> variables;
        for (const auto& variableObject : ExtractTopLevelObjectArray(message, "variables")) {
            const auto variablesReference = ExtractJsonInt(variableObject, "variablesReference").value_or(0);
            variables.push_back({
                    ExtractJsonString(variableObject, "name").value_or("value"),
                    ExtractJsonString(variableObject, "value").value_or(""),
                    ExtractJsonString(variableObject, "type").value_or(""),
                    variablesReference,
                    false,
                    variablesReference == 0,
                    {}
            });
        }

        return variables;
    }

    DebuggerScope* FindScopeByName(std::vector<DebuggerScope>& scopes, const std::string& name) {
        auto scopeIt = std::find_if(scopes.begin(), scopes.end(), [&name](const DebuggerScope& scope) {
            return scope.name == name;
        });
        return scopeIt != scopes.end() ? &(*scopeIt) : nullptr;
    }

    DebuggerVariable* FindVariableByName(std::vector<DebuggerVariable>& variables, const std::string& name) {
        for (auto& variable : variables) {
            if (variable.name == name) {
                return &variable;
            }

            if (auto* child = FindVariableByName(variable.variables, name)) {
                return child;
            }
        }

        return nullptr;
    }

    bool HasVisibleVariableNamed(const std::vector<DebuggerVariable>& variables, const std::string& name) {
        for (const auto& variable : variables) {
            if (variable.name == name || HasVisibleVariableNamed(variable.variables, name)) {
                return true;
            }
        }

        return false;
    }

    bool HasVisibleVariableNamed(const std::vector<DebuggerScope>& scopes, const std::string& name) {
        for (const auto& scope : scopes) {
            if (HasVisibleVariableNamed(scope.variables, name)) {
                return true;
            }
        }

        return false;
    }

    std::string TrimAscii(const std::string& value) {
        std::size_t start = 0;
        while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start])) != 0) {
            ++start;
        }

        std::size_t end = value.size();
        while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1])) != 0) {
            --end;
        }

        return value.substr(start, end - start);
    }

    bool IsIdentifier(const std::string& value) {
        if (value.empty() || (std::isalpha(static_cast<unsigned char>(value.front())) == 0 && value.front() != '_')) {
            return false;
        }

        for (const char c : value) {
            if (std::isalnum(static_cast<unsigned char>(c)) == 0 && c != '_') {
                return false;
            }
        }

        return true;
    }

    std::vector<std::string> SplitStructuredBindingNames(const std::string& value) {
        std::vector<std::string> names;
        std::size_t start = 0;
        while (start < value.size()) {
            const auto comma = value.find(',', start);
            const auto token = TrimAscii(value.substr(start, comma == std::string::npos ? std::string::npos : comma - start));
            if (IsIdentifier(token)) {
                names.push_back(token);
            }

            if (comma == std::string::npos) {
                break;
            }
            start = comma + 1;
        }

        return names;
    }

    std::string SanitizeCodeLine(const std::string& line, bool& inBlockComment) {
        std::string sanitized;
        sanitized.reserve(line.size());

        bool inString = false;
        bool escaped = false;
        char stringDelimiter = '\0';

        for (std::size_t i = 0; i < line.size(); ++i) {
            const char c = line[i];
            const char next = i + 1 < line.size() ? line[i + 1] : '\0';

            if (inBlockComment) {
                sanitized += ' ';
                if (c == '*' && next == '/') {
                    sanitized += ' ';
                    inBlockComment = false;
                    ++i;
                }
                continue;
            }

            if (inString) {
                sanitized += ' ';
                if (escaped) {
                    escaped = false;
                } else if (c == '\\') {
                    escaped = true;
                } else if (c == stringDelimiter) {
                    inString = false;
                }
                continue;
            }

            if (c == '/' && next == '/') {
                break;
            }

            if (c == '/' && next == '*') {
                sanitized += ' ';
                sanitized += ' ';
                inBlockComment = true;
                ++i;
                continue;
            }

            if (c == '"' || c == '\'') {
                sanitized += ' ';
                inString = true;
                stringDelimiter = c;
                escaped = false;
                continue;
            }

            sanitized += c;
        }

        return sanitized;
    }

    struct StructuredBindingMatch {
        std::vector<std::string> names;
        bool isControlStatement = false;
        bool opensScope = false;
    };

    std::optional<StructuredBindingMatch> ExtractStructuredBindingMatch(const std::string& line) {
        const auto openBracket = line.find('[');
        const auto closeBracket = openBracket == std::string::npos ? std::string::npos : line.find(']', openBracket + 1);
        if (openBracket == std::string::npos || closeBracket == std::string::npos) {
            return std::nullopt;
        }

        const auto autoPos = line.rfind("auto", openBracket);
        if (autoPos == std::string::npos) {
            return std::nullopt;
        }

        const bool validBoundaryBefore = autoPos == 0 ||
                                         (std::isalnum(static_cast<unsigned char>(line[autoPos - 1])) == 0 && line[autoPos - 1] != '_');
        const bool validBoundaryAfter = autoPos + 4 >= line.size() ||
                                        (std::isalnum(static_cast<unsigned char>(line[autoPos + 4])) == 0 && line[autoPos + 4] != '_');
        if (!validBoundaryBefore || !validBoundaryAfter) {
            return std::nullopt;
        }

        auto names = SplitStructuredBindingNames(line.substr(openBracket + 1, closeBracket - openBracket - 1));
        if (names.empty()) {
            return std::nullopt;
        }

        const auto prefix = line.substr(0, autoPos);
        const bool isControlStatement = prefix.find("for") != std::string::npos ||
                                        prefix.find("if") != std::string::npos ||
                                        prefix.find("while") != std::string::npos ||
                                        prefix.find("switch") != std::string::npos;
        const bool opensScope = line.find('{', closeBracket + 1) != std::string::npos;
        return StructuredBindingMatch{std::move(names), isControlStatement, opensScope};
    }

    std::vector<std::string> FindActiveStructuredBindingNames(const std::string& filePath, int currentLine) {
        if (filePath.empty() || currentLine <= 0 || !LittleCore::FileHelper::FileExists(filePath)) {
            return {};
        }

        struct ActiveBinding {
            int scopeDepth = 0;
            std::vector<std::string> names;
        };

        std::istringstream input(LittleCore::FileHelper::ReadAllText(filePath));
        std::string rawLine;
        std::vector<ActiveBinding> activeBindings;
        bool inBlockComment = false;
        int braceDepth = 0;
        int lineNumber = 0;

        while (std::getline(input, rawLine)) {
            ++lineNumber;
            const auto line = SanitizeCodeLine(rawLine, inBlockComment);

            if (const auto match = ExtractStructuredBindingMatch(line); match.has_value()) {
                activeBindings.push_back({
                        match->isControlStatement && match->opensScope ? braceDepth + 1 : braceDepth,
                        match->names
                });
            }

            braceDepth += static_cast<int>(std::count(line.begin(), line.end(), '{'));
            braceDepth -= static_cast<int>(std::count(line.begin(), line.end(), '}'));
            activeBindings.erase(std::remove_if(activeBindings.begin(),
                                                activeBindings.end(),
                                                [braceDepth](const ActiveBinding& binding) {
                                                    return binding.scopeDepth > braceDepth;
                                                }),
                                 activeBindings.end());

            if (lineNumber >= currentLine) {
                break;
            }
        }

        std::vector<std::string> names;
        for (const auto& binding : activeBindings) {
            for (const auto& name : binding.names) {
                if (std::find(names.begin(), names.end(), name) == names.end()) {
                    names.push_back(name);
                }
            }
        }

        return names;
    }

    DebuggerScope* EnsureScope(std::vector<DebuggerScope>& scopes, const std::string& name) {
        if (auto* scope = FindScopeByName(scopes, name)) {
            return scope;
        }

        scopes.push_back({
                name,
                false,
                0,
                false,
                true,
                {}
        });
        return &scopes.back();
    }
}

LldbSession::LldbSession(std::string executablePath, std::string workingDirectory) :
        executablePath(std::filesystem::path(std::move(executablePath)).lexically_normal().generic_string()),
        workingDirectory(std::filesystem::path(std::move(workingDirectory)).lexically_normal().generic_string()) {
}

LldbSession::~LldbSession() {
    CloseSession(true);
}

bool LldbSession::Start() {
    if (IsActive()) {
        return true;
    }

    const auto debugserverPath = ResolveDebugserverPath();
    if (debugserverPath.empty()) {
        statusText = "Missing bundled debugserver in /Users/jeppe/Jeppes/Scripting/clang18/bin";
        return false;
    }

    const auto lldbDapPath = ResolveLldbDapPath();
    if (lldbDapPath.empty()) {
        statusText = "Missing bundled lldb-dap in /Users/jeppe/Jeppes/Scripting/clang18/bin";
        return false;
    }

    int inputPipe[2] = {-1, -1};
    int outputPipe[2] = {-1, -1};
    int errorPipe[2] = {-1, -1};
    if (pipe(inputPipe) != 0 || pipe(outputPipe) != 0 || pipe(errorPipe) != 0) {
        statusText = "Failed to create pipes for LLDB: " + std::string(std::strerror(errno));
        if (inputPipe[0] >= 0) close(inputPipe[0]);
        if (inputPipe[1] >= 0) close(inputPipe[1]);
        if (outputPipe[0] >= 0) close(outputPipe[0]);
        if (outputPipe[1] >= 0) close(outputPipe[1]);
        if (errorPipe[0] >= 0) close(errorPipe[0]);
        if (errorPipe[1] >= 0) close(errorPipe[1]);
        return false;
    }

    const pid_t pid = fork();
    if (pid < 0) {
        statusText = "Failed to launch LLDB: " + std::string(std::strerror(errno));
        close(inputPipe[0]);
        close(inputPipe[1]);
        close(outputPipe[0]);
        close(outputPipe[1]);
        close(errorPipe[0]);
        close(errorPipe[1]);
        return false;
    }

    if (pid == 0) {
        dup2(inputPipe[0], STDIN_FILENO);
        dup2(outputPipe[1], STDOUT_FILENO);
        dup2(errorPipe[1], STDERR_FILENO);

        close(inputPipe[0]);
        close(inputPipe[1]);
        close(outputPipe[0]);
        close(outputPipe[1]);
        close(errorPipe[0]);
        close(errorPipe[1]);

        if (!workingDirectory.empty()) {
            chdir(workingDirectory.c_str());
        }

        setenv("LLDB_DEBUGSERVER_PATH", debugserverPath.c_str(), 1);
        execl(lldbDapPath.c_str(), lldbDapPath.c_str(), static_cast<char*>(nullptr));
        _exit(127);
    }

    close(inputPipe[0]);
    close(outputPipe[1]);
    close(errorPipe[1]);

    fcntl(outputPipe[0], F_SETFL, fcntl(outputPipe[0], F_GETFL, 0) | O_NONBLOCK);
    fcntl(errorPipe[0], F_SETFL, fcntl(errorPipe[0], F_GETFL, 0) | O_NONBLOCK);

    dapPid = static_cast<int>(pid);
    dapInputFd = inputPipe[1];
    dapOutputFd = outputPipe[0];
    dapErrorFd = errorPipe[0];
    inferiorPid = 0;
    activeThreadId = 0;
    initializeSucceeded = false;
    receivedInitializedEvent = false;
    configurationDoneSent = false;
    inferiorRunning = false;
    inferiorStopped = false;
    attachedToExistingProcess = false;
    hasCurrentLocation = false;
    hasExitCode = false;
    keepInferiorAliveOnExit = false;
    disconnectRequested = false;
    shutdownAfterStartupFailure = false;
    exitCode = 0;
    currentLocationFile.clear();
    currentLocationLine = 0;
    statusText = "Starting LLDB";
    consoleOutput.clear();
    readBuffer.clear();
    pendingStopReason.clear();
    pendingCommands.clear();
    knownBreakpointFiles.clear();
    ClearCurrentFrameData();
    nextRequestSeq = 1;

    SendRequest("initialize",
                "{\"adapterID\":\"EditorHub\",\"clientID\":\"EditorHub\",\"pathFormat\":\"path\",\"linesStartAt1\":true,\"columnsStartAt1\":true,\"supportsRunInTerminalRequest\":false}",
                CommandKind::Initialize,
                "Connecting to LLDB");
    return true;
}

void LldbSession::Poll() {
    if (!IsActive()) {
        return;
    }

    char buffer[4096];
    while (dapErrorFd >= 0) {
        const auto bytesRead = read(dapErrorFd, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            AppendConsoleOutput(std::string(buffer, static_cast<std::size_t>(bytesRead)));
            continue;
        }

        if (bytesRead < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            break;
        }

        if (bytesRead == 0 || (bytesRead < 0 && errno != EINTR)) {
            break;
        }
    }

    while (dapOutputFd >= 0) {
        const auto bytesRead = read(dapOutputFd, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            readBuffer.append(buffer, static_cast<std::size_t>(bytesRead));
            continue;
        }

        if (bytesRead < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            break;
        }

        if (bytesRead == 0 || (bytesRead < 0 && errno != EINTR)) {
            break;
        }
    }

    while (true) {
        const auto headerEnd = readBuffer.find("\r\n\r\n");
        if (headerEnd == std::string::npos) {
            break;
        }

        const auto header = readBuffer.substr(0, headerEnd);
        const auto contentLengthPos = header.find("Content-Length:");
        if (contentLengthPos == std::string::npos) {
            readBuffer.erase(0, headerEnd + 4);
            continue;
        }

        std::size_t valueStart = contentLengthPos + std::strlen("Content-Length:");
        valueStart = SkipWhitespace(header, valueStart);
        std::size_t valueEnd = valueStart;
        while (valueEnd < header.size() && std::isdigit(static_cast<unsigned char>(header[valueEnd])) != 0) {
            ++valueEnd;
        }

        const auto bodyLength = static_cast<std::size_t>(std::stoul(header.substr(valueStart, valueEnd - valueStart)));
        const auto bodyStart = headerEnd + 4;
        if (readBuffer.size() < bodyStart + bodyLength) {
            break;
        }

        auto body = readBuffer.substr(bodyStart, bodyLength);
        readBuffer.erase(0, bodyStart + bodyLength);
        HandleMessage(body);
    }

    int status = 0;
    const auto waitResult = waitpid(dapPid, &status, WNOHANG);
    if (waitResult == static_cast<pid_t>(dapPid)) {
        CloseSession(false);
        if (statusText.empty()) {
            statusText = "LLDB exited";
        }
    }
}

void LldbSession::SetSourceBreakpoints(const std::vector<SourceBreakpoint>& breakpoints) {
    sourceBreakpoints = breakpoints;
    std::sort(sourceBreakpoints.begin(), sourceBreakpoints.end(), [](const SourceBreakpoint& left, const SourceBreakpoint& right) {
        if (left.filePath != right.filePath) {
            return left.filePath < right.filePath;
        }
        return left.line < right.line;
    });
    sourceBreakpoints.erase(std::unique(sourceBreakpoints.begin(), sourceBreakpoints.end()), sourceBreakpoints.end());

    if (IsActive() && receivedInitializedEvent) {
        RefreshBreakpoints();
    }
}

void LldbSession::Launch() {
    requestedAction = RequestedAction::Launch;
    requestedAttachPid = 0;
    keepInferiorAliveOnExit = false;
    attachedToExistingProcess = false;

    if (!Start()) {
        return;
    }

    if (initializeSucceeded) {
        BeginRequestedSession();
    }
}

void LldbSession::Attach(int pid) {
    requestedAction = RequestedAction::Attach;
    requestedAttachPid = pid;
    keepInferiorAliveOnExit = true;
    attachedToExistingProcess = true;
    inferiorPid = pid;

    if (!Start()) {
        return;
    }

    if (initializeSucceeded) {
        BeginRequestedSession();
    }
}

void LldbSession::Continue() {
    if (!IsActive() || activeThreadId == 0) {
        return;
    }

    SendRequest("continue",
                "{\"threadId\":" + std::to_string(activeThreadId) + "}",
                CommandKind::Continue,
                "Continuing execution");
}

void LldbSession::Pause() {
    if (!IsActive() || activeThreadId == 0) {
        RequestThreads();
        return;
    }

    SendRequest("pause",
                "{\"threadId\":" + std::to_string(activeThreadId) + "}",
                CommandKind::Pause,
                "Pausing execution");
}

void LldbSession::StepInto() {
    if (!IsActive() || activeThreadId == 0) {
        return;
    }

    SendRequest("stepIn",
                "{\"threadId\":" + std::to_string(activeThreadId) + "}",
                CommandKind::StepInto,
                "Stepping into");
}

void LldbSession::StepOver() {
    if (!IsActive() || activeThreadId == 0) {
        return;
    }

    SendRequest("next",
                "{\"threadId\":" + std::to_string(activeThreadId) + "}",
                CommandKind::StepOver,
                "Stepping over");
}

void LldbSession::StepOut() {
    if (!IsActive() || activeThreadId == 0) {
        return;
    }

    SendRequest("stepOut",
                "{\"threadId\":" + std::to_string(activeThreadId) + "}",
                CommandKind::StepOut,
                "Stepping out");
}

void LldbSession::DetachAndQuit() {
    if (!IsActive()) {
        return;
    }

    keepInferiorAliveOnExit = true;
    QueueDisconnect(false, "Detaching LLDB");
}

void LldbSession::KillInferiorAndQuit() {
    if (!IsActive()) {
        return;
    }

    keepInferiorAliveOnExit = false;
    QueueDisconnect(true, "Stopping debugged process");
}

bool LldbSession::IsActive() const {
    return dapPid != 0;
}

bool LldbSession::HasInferiorProcess() const {
    return inferiorPid != 0;
}

int LldbSession::InferiorPid() const {
    return inferiorPid;
}

bool LldbSession::IsInferiorRunning() const {
    return inferiorRunning;
}

bool LldbSession::IsInferiorStopped() const {
    return inferiorStopped;
}

bool LldbSession::IsAttachedToExistingProcess() const {
    return attachedToExistingProcess;
}

bool LldbSession::HasCurrentLocation() const {
    return hasCurrentLocation;
}

const std::string& LldbSession::CurrentLocationFile() const {
    return currentLocationFile;
}

int LldbSession::CurrentLocationLine() const {
    return currentLocationLine;
}

bool LldbSession::HasExitCode() const {
    return hasExitCode;
}

int LldbSession::ExitCode() const {
    return exitCode;
}

const std::string& LldbSession::StatusText() const {
    return statusText;
}

const std::string& LldbSession::ConsoleOutput() const {
    return consoleOutput;
}

const std::vector<DebuggerScope>& LldbSession::CurrentScopes() const {
    return currentScopes;
}

void LldbSession::EnsureVariableChildrenLoaded(int variablesReference) {
    EnsureVariablesLoaded(variablesReference, false);
}

void LldbSession::CloseSession(bool terminateProcess) {
    if (dapInputFd >= 0) {
        close(dapInputFd);
        dapInputFd = -1;
    }

    if (dapOutputFd >= 0) {
        close(dapOutputFd);
        dapOutputFd = -1;
    }

    if (dapErrorFd >= 0) {
        close(dapErrorFd);
        dapErrorFd = -1;
    }

    if (terminateProcess && inferiorPid != 0 && !keepInferiorAliveOnExit) {
        kill(inferiorPid, SIGTERM);
    }

    if (dapPid != 0) {
        if (terminateProcess) {
            kill(dapPid, SIGTERM);
        }

        int status = 0;
        const auto waitResult = waitpid(dapPid, &status, terminateProcess ? 0 : WNOHANG);
        if (terminateProcess && waitResult == 0) {
            kill(dapPid, SIGKILL);
            waitpid(dapPid, &status, 0);
        }
    }

    dapPid = 0;
    pendingCommands.clear();
    inferiorRunning = false;
    inferiorStopped = false;
    activeThreadId = 0;
    ClearCurrentFrameData();
    if (!keepInferiorAliveOnExit) {
        inferiorPid = 0;
    }
}

void LldbSession::ClearCurrentFrameData() {
    currentFrameId = 0;
    hasCurrentLocation = false;
    currentLocationFile.clear();
    currentLocationLine = 0;
    currentScopes.clear();
}

void LldbSession::BeginRequestedSession() {
    if (!initializeSucceeded || requestedAction == RequestedAction::None) {
        return;
    }

    receivedInitializedEvent = false;
    configurationDoneSent = false;
    disconnectRequested = false;
    shutdownAfterStartupFailure = false;
    hasExitCode = false;
    activeThreadId = 0;
    pendingStopReason.clear();
    knownBreakpointFiles.clear();
    ++currentStopGeneration;
    ClearCurrentFrameData();

    if (requestedAction == RequestedAction::Launch) {
        std::string arguments = "{\"name\":\"" + EscapeJsonString(std::filesystem::path(executablePath).filename().string()) + "\"";
        arguments += ",\"type\":\"lldb\",\"request\":\"launch\"";
        arguments += ",\"program\":\"" + EscapeJsonString(executablePath) + "\"";
        if (!workingDirectory.empty()) {
            arguments += ",\"cwd\":\"" + EscapeJsonString(workingDirectory) + "\"";
        }
        arguments += ",\"initCommands\":[\"settings set target.experimental.inject-local-vars true\"]";
        arguments += ",\"stopOnEntry\":false}";

        SendRequest("launch", arguments, CommandKind::Launch, "Launching under LLDB");
    } else if (requestedAction == RequestedAction::Attach) {
        std::string arguments = "{\"name\":\"" + EscapeJsonString(std::filesystem::path(executablePath).filename().string()) + "\"";
        arguments += ",\"type\":\"lldb\",\"request\":\"attach\"";
        arguments += ",\"program\":\"" + EscapeJsonString(executablePath) + "\"";
        arguments += ",\"pid\":" + std::to_string(requestedAttachPid);
        arguments += ",\"initCommands\":[\"settings set target.experimental.inject-local-vars true\"]";
        arguments += ",\"waitFor\":false}";

        SendRequest("attach", arguments, CommandKind::Attach, "Attaching LLDB");
    }

    requestedAction = RequestedAction::None;
}

void LldbSession::RefreshBreakpoints() {
    if (!IsActive() || !receivedInitializedEvent) {
        return;
    }

    std::vector<std::string> filesToUpdate = knownBreakpointFiles;
    for (const auto& breakpoint : sourceBreakpoints) {
        if (std::find(filesToUpdate.begin(), filesToUpdate.end(), breakpoint.filePath) == filesToUpdate.end()) {
            filesToUpdate.push_back(breakpoint.filePath);
        }
    }

    std::sort(filesToUpdate.begin(), filesToUpdate.end());

    std::vector<std::string> updatedFiles;
    for (const auto& filePath : filesToUpdate) {
        std::string arguments = "{\"source\":{\"path\":\"" + EscapeJsonString(filePath) + "\"},\"breakpoints\":[";
        bool first = true;
        for (const auto& breakpoint : sourceBreakpoints) {
            if (breakpoint.filePath != filePath) {
                continue;
            }

            if (!first) {
                arguments += ",";
            }
            arguments += "{\"line\":" + std::to_string(breakpoint.line) + "}";
            first = false;
        }
        arguments += "]}";

        SendRequest("setBreakpoints",
                    arguments,
                    CommandKind::SetBreakpoints,
                    "Updating breakpoints");

        if (!first) {
            updatedFiles.push_back(filePath);
        }
    }

    knownBreakpointFiles = std::move(updatedFiles);
}

void LldbSession::RequestThreads() {
    if (!IsActive()) {
        return;
    }

    SendRequest("threads", "", CommandKind::Threads, "Refreshing LLDB threads");
}

void LldbSession::RequestStackTrace(int threadId) {
    if (!IsActive() || threadId == 0) {
        return;
    }

    SendRequest("stackTrace",
                "{\"threadId\":" + std::to_string(threadId) + ",\"startFrame\":0,\"levels\":1}",
                CommandKind::StackTrace,
                "Fetching stop location",
                0,
                currentStopGeneration);
}

void LldbSession::RequestScopes(int frameId) {
    if (!IsActive() || frameId == 0) {
        return;
    }

    AppendConsoleOutput("[lldb-dap] scopes frameId=" + std::to_string(frameId) + "\n");
    SendRequest("scopes",
                "{\"frameId\":" + std::to_string(frameId) + "}",
                CommandKind::Scopes,
                "Fetching debugger scopes",
                frameId,
                currentStopGeneration);
}

void LldbSession::RequestVariables(int variablesReference, bool namedOnly) {
    if (!IsActive() || variablesReference == 0) {
        return;
    }

    AppendConsoleOutput("[lldb-dap] variables ref=" + std::to_string(variablesReference) + "\n");
    std::string arguments = "{\"variablesReference\":" + std::to_string(variablesReference);
    if (namedOnly) {
        arguments += ",\"filter\":\"named\"";
    }
    arguments += ",\"start\":0,\"count\":512}";
    SendRequest("variables",
                arguments,
                CommandKind::Variables,
                "Fetching debugger variables",
                variablesReference,
                currentStopGeneration);
}

void LldbSession::EnsureVariablesLoaded(int variablesReference, bool namedOnly) {
    if (!IsActive() || !inferiorStopped || variablesReference == 0) {
        return;
    }

    if (!BeginVariablesLoad(currentScopes, variablesReference)) {
        return;
    }

    RequestVariables(variablesReference, namedOnly);
}

void LldbSession::RequestEvaluate(const std::string& expression) {
    if (!IsActive() || !inferiorStopped || currentFrameId == 0 || expression.empty()) {
        return;
    }

    AppendConsoleOutput("[lldb-dap] evaluate " + expression + "\n");
    SendRequest("evaluate",
                "{\"expression\":\"" + EscapeJsonString(expression) +
                "\",\"frameId\":" + std::to_string(currentFrameId) +
                ",\"context\":\"watch\"}",
                CommandKind::Evaluate,
                "Evaluating debugger expression",
                0,
                currentStopGeneration,
                expression);
}

void LldbSession::QueueStructuredBindingEvaluations() {
    if (!inferiorStopped || currentFrameId == 0 || !hasCurrentLocation) {
        return;
    }

    auto names = FindActiveStructuredBindingNames(currentLocationFile, currentLocationLine);
    names.erase(std::remove_if(names.begin(), names.end(), [this](const std::string& name) {
                    return HasVisibleVariableNamed(currentScopes, name);
                }),
                names.end());

    if (names.empty()) {
        return;
    }

    auto* scope = EnsureScope(currentScopes, "Structured Bindings");
    for (const auto& name : names) {
        if (FindVariableByName(scope->variables, name) != nullptr) {
            continue;
        }

        scope->variables.push_back({
                name,
                "",
                "",
                0,
                true,
                false,
                {}
        });
        RequestEvaluate(name);
    }
}

void LldbSession::QueueDisconnect(bool terminateDebuggee, const std::string& description) {
    if (!IsActive() || disconnectRequested) {
        return;
    }

    disconnectRequested = true;
    SendRequest("disconnect",
                std::string("{\"terminateDebuggee\":") + (terminateDebuggee ? "true" : "false") + "}",
                CommandKind::Disconnect,
                description);
}

void LldbSession::SendRequest(const std::string& command,
                              const std::string& argumentsJson,
                              CommandKind kind,
                              std::string description,
                              int referenceId,
                              int stopGeneration,
                              std::string expression) {
    if (dapInputFd < 0) {
        return;
    }

    const int requestSeq = nextRequestSeq++;
    pendingCommands.push_back({requestSeq, PendingCommand{
            kind,
            std::move(description),
            std::move(expression),
            referenceId,
            stopGeneration < 0 ? currentStopGeneration : stopGeneration
    }});

    std::string payload = "{\"seq\":" + std::to_string(requestSeq) + ",\"type\":\"request\",\"command\":\"" + command + "\"";
    if (!argumentsJson.empty()) {
        payload += ",\"arguments\":" + argumentsJson;
    }
    payload += "}";

    std::string framedPayload = "Content-Length: " + std::to_string(payload.size()) + "\r\n\r\n" + payload;
    const char* data = framedPayload.c_str();
    std::size_t bytesRemaining = framedPayload.size();
    while (bytesRemaining > 0) {
        const auto bytesWritten = write(dapInputFd, data, bytesRemaining);
        if (bytesWritten < 0) {
            if (errno == EINTR) {
                continue;
            }

            statusText = "Failed to communicate with LLDB: " + std::string(std::strerror(errno));
            return;
        }

        data += bytesWritten;
        bytesRemaining -= static_cast<std::size_t>(bytesWritten);
    }
}

void LldbSession::HandleMessage(const std::string& message) {
    const auto type = ExtractTopLevelJsonString(message, "type").value_or("");
    if (type == "event") {
        HandleEvent(message);
        return;
    }

    if (type == "response") {
        HandleResponse(message);
    }
}

void LldbSession::HandleEvent(const std::string& message) {
    const auto event = ExtractTopLevelJsonString(message, "event").value_or("");
    if (event == "initialized") {
        receivedInitializedEvent = true;
        RefreshBreakpoints();

        if (!configurationDoneSent) {
            configurationDoneSent = true;
            SendRequest("configurationDone", "", CommandKind::ConfigurationDone, "Configuring LLDB");
        }

        if (shutdownAfterStartupFailure) {
            QueueDisconnect(false, "Closing LLDB");
        }
        return;
    }

    if (event == "process") {
        inferiorPid = ExtractJsonInt(message, "systemProcessId").value_or(inferiorPid);
        RequestThreads();
        return;
    }

    if (event == "output") {
        AppendConsoleOutput(ExtractJsonString(message, "output").value_or(""));
        return;
    }

    if (event == "continued") {
        inferiorRunning = true;
        inferiorStopped = false;
        ++currentStopGeneration;
        ClearCurrentFrameData();
        statusText = "Debugger running";
        return;
    }

    if (event == "stopped") {
        const auto threadId = ExtractJsonInt(message, "threadId").value_or(activeThreadId);
        const auto reason = ExtractJsonString(message, "reason").value_or("paused");
        const bool duplicateStop = inferiorStopped &&
                                   !inferiorRunning &&
                                   threadId == activeThreadId &&
                                   reason == pendingStopReason &&
                                   currentFrameId != 0;

        inferiorRunning = false;
        inferiorStopped = true;

        if (duplicateStop) {
            AppendConsoleOutput("[lldb-dap] duplicate stopped event ignored\n");
            return;
        }

        activeThreadId = threadId;
        pendingStopReason = reason;
        ClearCurrentFrameData();
        RequestStackTrace(activeThreadId);
        UpdateStopStatus();
        return;
    }

    if (event == "exited") {
        inferiorRunning = false;
        inferiorStopped = false;
        ++currentStopGeneration;
        ClearCurrentFrameData();
        inferiorPid = 0;
        hasExitCode = true;
        exitCode = ExtractJsonInt(message, "exitCode").value_or(0);
        statusText = exitCode == 0
                     ? "Program exited normally"
                     : "Program exited with code " + std::to_string(exitCode);
        return;
    }

    if (event == "terminated") {
        ++currentStopGeneration;
        ClearCurrentFrameData();
        if (statusText.empty()) {
            statusText = keepInferiorAliveOnExit ? "Debugger detached" : "Stopped debugging";
        }
        return;
    }
}

void LldbSession::HandleResponse(const std::string& message) {
    const auto requestSeq = ExtractTopLevelJsonInt(message, "request_seq").value_or(0);
    const auto command = ExtractTopLevelJsonString(message, "command").value_or("");
    const auto success = ExtractTopLevelJsonBool(message, "success").value_or(false);
    const auto errorMessage = ExtractTopLevelJsonString(message, "message").value_or("Unknown LLDB error");

    PendingCommand pendingCommand;
    auto pendingIt = std::find_if(pendingCommands.begin(), pendingCommands.end(), [requestSeq](const auto& entry) {
        return entry.first == requestSeq;
    });
    if (pendingIt != pendingCommands.end()) {
        pendingCommand = pendingIt->second;
        pendingCommands.erase(pendingIt);
    }

    const bool isFrameDataCommand = pendingCommand.kind == CommandKind::StackTrace ||
                                    pendingCommand.kind == CommandKind::Scopes ||
                                    pendingCommand.kind == CommandKind::Variables ||
                                    pendingCommand.kind == CommandKind::Evaluate;
    if (isFrameDataCommand && pendingCommand.stopGeneration != currentStopGeneration) {
        AppendConsoleOutput("[lldb-dap] dropped stale frame data response for " + pendingCommand.description + "\n");
        return;
    }

    if (!success) {
        if (pendingCommand.kind == CommandKind::Variables) {
            FinishVariablesLoad(currentScopes, pendingCommand.referenceId, {});
        } else if (pendingCommand.kind == CommandKind::Evaluate) {
            if (auto* scope = FindScopeByName(currentScopes, "Structured Bindings")) {
                if (auto* variable = FindVariableByName(scope->variables, pendingCommand.expression)) {
                    variable->value = "<unavailable>";
                    variable->type.clear();
                    variable->variablesReference = 0;
                    variable->variablesLoading = false;
                    variable->variablesLoaded = true;
                }
            }

            AppendConsoleOutput("[lldb-dap] " + pendingCommand.description + " failed for \"" +
                                pendingCommand.expression + "\": " + errorMessage + "\n");
            return;
        }

        statusText = pendingCommand.description.empty()
                     ? errorMessage
                     : (pendingCommand.description + " failed: " + errorMessage);
        AppendConsoleOutput("[lldb-dap] " + pendingCommand.description + " failed: " + errorMessage + "\n");

        if (pendingCommand.kind == CommandKind::Attach) {
            inferiorRunning = true;
            inferiorStopped = false;
        }

        if (pendingCommand.kind == CommandKind::Launch || pendingCommand.kind == CommandKind::Attach) {
            shutdownAfterStartupFailure = true;
            if (receivedInitializedEvent) {
                QueueDisconnect(false, "Closing LLDB");
            }
        }
        return;
    }

    switch (pendingCommand.kind) {
        case CommandKind::Initialize:
            initializeSucceeded = true;
            statusText = "LLDB ready";
            BeginRequestedSession();
            break;
        case CommandKind::Launch:
            statusText = "Launching under LLDB";
            RequestThreads();
            break;
        case CommandKind::Attach:
            statusText = inferiorPid == 0
                         ? "Attached LLDB"
                         : "Attached LLDB to process " + std::to_string(inferiorPid);
            RequestThreads();
            break;
        case CommandKind::Continue:
            statusText = "Continuing execution";
            break;
        case CommandKind::Pause:
            statusText = "Pausing execution";
            break;
        case CommandKind::StepInto:
            statusText = "Stepping into";
            break;
        case CommandKind::StepOver:
            statusText = "Stepping over";
            break;
        case CommandKind::StepOut:
            statusText = "Stepping out";
            break;
        case CommandKind::Threads: {
            const auto threadsPos = message.find("\"threads\"");
            if (threadsPos != std::string::npos) {
                activeThreadId = ExtractJsonInt(message, "id", threadsPos).value_or(activeThreadId);
            }
            break;
        }
        case CommandKind::StackTrace: {
            const auto framesPos = message.find("\"stackFrames\"");
            if (framesPos != std::string::npos) {
                currentFrameId = ExtractJsonInt(message, "id", framesPos).value_or(0);
                auto path = ExtractJsonString(message, "path", framesPos);
                auto line = ExtractJsonInt(message, "line", framesPos);
                if (path.has_value() && line.has_value()) {
                    hasCurrentLocation = true;
                    currentLocationFile = std::filesystem::path(*path).lexically_normal().generic_string();
                    currentLocationLine = *line;
                }

                if (currentFrameId != 0) {
                    RequestScopes(currentFrameId);
                }
            }
            UpdateStopStatus();
            break;
        }
        case CommandKind::Scopes: {
            currentScopes.clear();
            for (const auto& scopeObject : ExtractTopLevelObjectArray(message, "scopes")) {
                const auto name = ExtractJsonString(scopeObject, "name").value_or("Scope");
                const auto expensive = ExtractJsonBool(scopeObject, "expensive").value_or(false);
                const auto variablesReference = ExtractJsonInt(scopeObject, "variablesReference").value_or(0);

                if (!ScopeShouldLoad(name, expensive)) {
                    continue;
                }

                currentScopes.push_back({
                        name,
                        expensive,
                        variablesReference,
                        false,
                        variablesReference == 0,
                        {}
                });

                if (variablesReference != 0) {
                    EnsureVariablesLoaded(variablesReference, true);
                }
            }
            AppendConsoleOutput("[lldb-dap] scopes loaded=" + std::to_string(currentScopes.size()) + "\n");
            QueueStructuredBindingEvaluations();
            break;
        }
        case CommandKind::Variables: {
            auto variables = ParseDebuggerVariables(message);
            const auto variableCount = variables.size();
            if (!FinishVariablesLoad(currentScopes, pendingCommand.referenceId, std::move(variables))) {
                AppendConsoleOutput("[lldb-dap] variables response with unknown ref=" + std::to_string(pendingCommand.referenceId) + "\n");
                break;
            }

            AppendConsoleOutput("[lldb-dap] variables loaded ref=" + std::to_string(pendingCommand.referenceId) +
                                " count=" + std::to_string(variableCount) + "\n");
            break;
        }
        case CommandKind::Evaluate: {
            auto* scope = EnsureScope(currentScopes, "Structured Bindings");
            auto* variable = FindVariableByName(scope->variables, pendingCommand.expression);
            if (variable == nullptr) {
                scope->variables.push_back({
                        pendingCommand.expression,
                        "",
                        "",
                        0,
                        true,
                        false,
                        {}
                });
                variable = &scope->variables.back();
            }

            const auto bodyPos = message.find("\"body\"");
            const auto result = ExtractJsonString(message, "result", bodyPos).value_or("");
            const auto type = ExtractJsonString(message, "type", bodyPos).value_or("");
            const auto variablesReference = ExtractJsonInt(message, "variablesReference", bodyPos).value_or(0);

            variable->value = result;
            variable->type = type;
            variable->variablesReference = variablesReference;
            variable->variablesLoading = false;
            variable->variablesLoaded = variablesReference == 0;
            variable->variables.clear();

            AppendConsoleOutput("[lldb-dap] evaluate loaded \"" + pendingCommand.expression + "\"\n");
            break;
        }
        case CommandKind::Disconnect:
            inferiorRunning = false;
            inferiorStopped = false;
            statusText = keepInferiorAliveOnExit ? "Debugger detached" : "Stopped debugging";
            break;
        case CommandKind::ConfigurationDone:
            if (command == "configurationDone" && statusText == "LLDB ready") {
                statusText = "LLDB configured";
            }
            break;
        case CommandKind::SetBreakpoints:
        case CommandKind::Generic:
            break;
    }
}

void LldbSession::UpdateStopStatus() {
    if (!inferiorStopped) {
        return;
    }

    if (pendingStopReason == "breakpoint") {
        statusText = hasCurrentLocation
                     ? "Paused at " + currentLocationFile + ":" + std::to_string(currentLocationLine) + " (breakpoint hit)"
                     : "Paused at a breakpoint";
        return;
    }

    if (pendingStopReason == "entry") {
        statusText = hasCurrentLocation
                     ? "Paused at " + currentLocationFile + ":" + std::to_string(currentLocationLine) + " (entry)"
                     : "Paused at entry";
        return;
    }

    if (pendingStopReason == "exception") {
        statusText = hasCurrentLocation
                     ? "Paused on exception at " + currentLocationFile + ":" + std::to_string(currentLocationLine)
                     : "Paused on exception";
        return;
    }

    if (pendingStopReason == "pause") {
        statusText = hasCurrentLocation
                     ? "Paused at " + currentLocationFile + ":" + std::to_string(currentLocationLine)
                     : "Paused";
        return;
    }

    if (pendingStopReason == "step") {
        statusText = hasCurrentLocation
                     ? "Paused at " + currentLocationFile + ":" + std::to_string(currentLocationLine)
                     : "Paused after stepping";
        return;
    }

    statusText = hasCurrentLocation
                 ? "Paused at " + currentLocationFile + ":" + std::to_string(currentLocationLine)
                 : "Paused";
}

void LldbSession::AppendConsoleOutput(const std::string& text) {
    consoleOutput += text;
    if (consoleOutput.size() <= MaxConsoleOutputSize) {
        return;
    }

    consoleOutput.erase(0, consoleOutput.size() - MaxConsoleOutputSize);
}

std::string LldbSession::EscapeJsonString(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size() + 8);

    for (const char c : value) {
        switch (c) {
            case '\\':
            case '"':
                escaped += '\\';
                escaped += c;
                break;
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            case '\t':
                escaped += "\\t";
                break;
            default:
                escaped += c;
                break;
        }
    }

    return escaped;
}

std::string LldbSession::ResolveLldbDapPath() {
    constexpr const char* bundledPath = "/Users/jeppe/Jeppes/Scripting/clang18/bin/lldb-dap";
    return std::filesystem::exists(bundledPath) ? bundledPath : std::string{};
}

std::string LldbSession::ResolveDebugserverPath() {
    constexpr const char* bundledPath = "/Users/jeppe/Jeppes/Scripting/clang18/bin/debugserver";
    return std::filesystem::exists(bundledPath) ? bundledPath : std::string{};
}
