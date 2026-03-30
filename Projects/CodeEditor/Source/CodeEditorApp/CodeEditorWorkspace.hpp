//
// Created by Jeppe Nielsen on 30/03/2026.
//

#pragma once
#include "CodeEditorAutocomplete.hpp"
#include "TextEditor.hpp"
#include "imgui.h"
#include <cstdint>
#include <string>
#include <vector>

class CodeEditorWorkspace {
public:
    void OpenFile(const std::string& path);
    void Draw(CodeEditorAutocomplete& autocomplete, ImFont* codeFont);

    void SetStatusText(std::string text);
    const std::string& StatusText() const;
    const std::string& ActivePath() const;

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

    struct Document {
        std::string path;
        std::string title;
        std::string savedText;
        LittleCore::TextEditor editor;
        bool isDirty = false;
        bool requestSelection = false;
        CompletionPopupState completion;
    };

    std::vector<Document> documents;
    std::string activePath;
    std::string statusText;

    bool SaveDocument(Document& document);
    void CloseCompletion(Document& document);
    void RequestCompletion(Document& document, CodeEditorAutocomplete& autocomplete);
    void ApplyCompletionResult(const CodeEditorAutocompleteResult& result);
    void PumpCompletionResults(CodeEditorAutocomplete& autocomplete);
    void ApplySelectedCompletion(Document& document);
    void DrawDocument(Document& document, int index, CodeEditorAutocomplete& autocomplete, ImFont* codeFont);
    void DrawCompletionPopup(Document& document, const std::string& popupId);
    void DrawStatusBar(const Document& document) const;
};
