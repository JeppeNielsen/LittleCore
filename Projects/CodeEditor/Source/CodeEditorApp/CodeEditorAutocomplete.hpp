//
// Created by Jeppe Nielsen on 30/03/2026.
//

#pragma once
#include "TextEditor.hpp"
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

struct CodeEditorCompletionCandidate {
    std::string insertText;
    std::string displayText;
    std::string returnType;
    std::vector<std::string> parameters;
    unsigned clangPriority = 0;
    bool isType = false;
    bool isCurrentFileSymbol = false;
    bool isTemplateParameter = false;
    bool isFunctionParameter = false;
    bool isLocalVariable = false;
    bool isField = false;
    bool appendOpeningParenthesis = false;
};

struct CodeEditorAutocompleteResult {
    std::uint64_t requestId = 0;
    std::string filePath;
    LittleCore::TextEditor::Coordinates cursorPosition;
    std::string prefix;
    std::vector<CodeEditorCompletionCandidate> candidates;
};

struct CodeEditorSignatureHelpResult {
    std::uint64_t requestId = 0;
    std::string filePath;
    std::size_t openParenthesisOffset = 0;
    std::string functionName;
    std::vector<std::string> parameters;
};

class CodeEditorAutocomplete {
public:
    explicit CodeEditorAutocomplete(std::string workspaceRoot = {});
    ~CodeEditorAutocomplete();

    CodeEditorAutocomplete(const CodeEditorAutocomplete&) = delete;
    CodeEditorAutocomplete& operator=(const CodeEditorAutocomplete&) = delete;

    void SetWorkspaceRoot(std::string workspaceRoot);
    void SetProjectRoot(std::string projectRoot);
    void SetCodeFiles(const std::vector<std::string>& codeFiles);

    std::uint64_t QueueCompletion(const std::string& filePath, const LittleCore::TextEditor& editor);
    std::uint64_t QueueSignatureHelp(const std::string& filePath,
                                     const LittleCore::TextEditor& editor,
                                     std::size_t openParenthesisOffset);
    std::optional<CodeEditorAutocompleteResult> TakeCompletedResult();
    std::optional<CodeEditorSignatureHelpResult> TakeCompletedSignatureHelpResult();

private:
    struct CompletionRequest {
        std::uint64_t requestId = 0;
        std::string filePath;
        std::string sourceText;
        LittleCore::TextEditor::Coordinates cursorPosition;
        std::string prefix;
    };

    struct SignatureHelpRequest {
        std::uint64_t requestId = 0;
        std::string filePath;
        std::string sourceText;
        LittleCore::TextEditor::Coordinates lookupCursorPosition;
        std::size_t openParenthesisOffset = 0;
        std::string callableName;
    };

    struct ContextSnapshot {
        std::string workspaceRoot;
        std::string projectRoot;
        std::vector<std::string> codeIncludeDirectories;
    };

    void WorkerMain();
    ContextSnapshot CreateContextSnapshotLocked() const;

    mutable std::mutex mutex;
    std::condition_variable condition;
    bool stopRequested = false;
    std::uint64_t nextRequestId = 1;
    std::optional<CompletionRequest> pendingCompletionRequest;
    std::optional<SignatureHelpRequest> pendingSignatureHelpRequest;
    std::optional<CodeEditorAutocompleteResult> completedResult;
    std::optional<CodeEditorSignatureHelpResult> completedSignatureHelpResult;

    std::string workspaceRoot;
    std::string projectRoot;
    std::vector<std::string> codeIncludeDirectories;
    std::thread worker;
};
