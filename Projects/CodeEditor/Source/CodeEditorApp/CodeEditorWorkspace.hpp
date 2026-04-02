//
// Created by Jeppe Nielsen on 30/03/2026.
//

#pragma once
#include "CodeEditorAutocomplete.hpp"
#include "TextEditor.hpp"
#include "../../../EditorHub/Source/Programs/SourceBreakpoint.hpp"
#include "imgui.h"
#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class CodeEditorWorkspace {
public:
    void OpenFile(const std::string& path);
    void OpenFileAtLine(const std::string& path, int line);
    void RemovePath(const std::string& path);
    void SetDisplayRootPath(std::string rootPath);
    void Draw(CodeEditorAutocomplete& autocomplete, ImFont* codeFont);

    void SetStatusText(std::string text);
    const std::string& StatusText() const;
    const std::string& ActivePath() const;
    std::vector<SourceBreakpoint> SourceBreakpoints() const;
    bool ConsumeBreakpointsChanged();

private:
    struct CompletionPopupState {
        bool isOpen = false;
        bool isLoading = false;
        int selectedIndex = 0;
        std::uint64_t requestId = 0;
        std::string prefix;
        LittleCore::TextEditor::Coordinates cursorPosition;
        std::vector<CodeEditorCompletionCandidate> candidates;
    };

    struct SignatureHelpState {
        bool isOpen = false;
        std::uint64_t requestId = 0;
        std::string functionName;
        std::vector<std::string> parameters;
        std::size_t openParenthesisOffset = std::numeric_limits<std::size_t>::max();
        int activeParameterIndex = 0;
        bool lookupFailed = false;
    };

    struct Document {
        std::string path;
        std::string title;
        std::string savedText;
        LittleCore::TextEditor editor;
        bool isDirty = false;
        bool requestSelection = false;
        std::optional<LittleCore::TextEditor::Coordinates> pendingCursorPosition;
        CompletionPopupState completion;
        SignatureHelpState signatureHelp;
    };

    std::vector<Document> documents;
    std::string displayRootPath;
    std::string activePath;
    std::string statusText;
    std::unordered_map<std::string, LittleCore::TextEditor::Breakpoints> fileBreakpoints;
    bool breakpointsChanged = false;

    bool SaveDocument(Document& document);
    Document& OpenOrCreateDocument(const std::string& path);
    void ToggleBreakpoint(Document& document, int line);
    void SyncBreakpointsFromEditor(const Document& document);
    void CloseCompletion(Document& document);
    void CloseSignatureHelp(Document& document);
    void OpenSignatureHelp(Document& document, const CodeEditorCompletionCandidate& candidate, std::size_t openParenthesisOffset);
    void RequestSignatureHelp(Document& document, CodeEditorAutocomplete& autocomplete, std::size_t openParenthesisOffset);
    void SyncSignatureHelp(Document& document, CodeEditorAutocomplete& autocomplete, bool allowOpen);
    void RequestCompletion(Document& document, CodeEditorAutocomplete& autocomplete);
    void ApplyCompletionResult(const CodeEditorAutocompleteResult& result);
    void ApplySignatureHelpResult(const CodeEditorSignatureHelpResult& result);
    void PumpCompletionResults(CodeEditorAutocomplete& autocomplete);
    void PumpSignatureHelpResults(CodeEditorAutocomplete& autocomplete);
    void ApplySelectedCompletion(Document& document);
    void DrawDocument(Document& document, int index, CodeEditorAutocomplete& autocomplete, ImFont* codeFont);
    void DrawCompletionPopup(Document& document, const std::string& popupId);
    void UpdateSignatureHelp(Document& document);
    void DrawSignatureHelpPopup(const Document& document, const std::string& popupId) const;
    void DrawStatusBar(const Document& document) const;
};
