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
    unsigned clangPriority = 0;
    bool isCurrentFileSymbol = false;
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
    std::optional<CodeEditorAutocompleteResult> TakeCompletedResult();

private:
    struct CompletionRequest {
        std::uint64_t requestId = 0;
        std::string filePath;
        std::string sourceText;
        LittleCore::TextEditor::Coordinates cursorPosition;
        std::string prefix;
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
    std::optional<CompletionRequest> pendingRequest;
    std::optional<CodeEditorAutocompleteResult> completedResult;

    std::string workspaceRoot;
    std::string projectRoot;
    std::vector<std::string> codeIncludeDirectories;
    std::thread worker;
};
