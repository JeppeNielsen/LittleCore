//
// Created by Jeppe Nielsen on 30/03/2026.
//

#include "CodeEditorWorkspace.hpp"
#include "FileHelper.hpp"
#include "imgui.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <optional>
#include <utility>

using namespace LittleCore;

namespace {
    constexpr float CompletionPopupWidth = 420.0f;
    constexpr float CompletionPopupMaxHeight = 220.0f;
    constexpr float CompletionPopupMinHeight = 72.0f;
    constexpr float CompletionPopupVerticalOffset = 4.0f;

    struct CompletionRequest {
        TextEditor::Coordinates cursorPosition;
        std::string prefix;
    };

    bool IsIdentifierCharacter(char value) {
        const auto c = static_cast<unsigned char>(value);
        return std::isalnum(c) != 0 || c == '_';
    }

    std::size_t CursorOffsetFromCoordinates(const std::string& text, const TextEditor::Coordinates& coordinates) {
        std::size_t offset = 0;
        int currentLine = 0;

        while (currentLine < coordinates.mLine && offset < text.size()) {
            const auto nextLine = text.find('\n', offset);
            if (nextLine == std::string::npos) {
                return text.size();
            }

            offset = nextLine + 1;
            ++currentLine;
        }

        const auto lineEnd = text.find('\n', offset);
        const auto clampedLineEnd = lineEnd == std::string::npos ? text.size() : lineEnd;
        return std::min(offset + static_cast<std::size_t>(coordinates.mColumn), clampedLineEnd);
    }

    CompletionRequest BuildCompletionRequest(const TextEditor& editor) {
        CompletionRequest request;
        request.cursorPosition = editor.GetCursorPosition();

        const auto text = editor.GetText();
        auto cursorOffset = CursorOffsetFromCoordinates(text, request.cursorPosition);
        auto prefixStart = cursorOffset;

        while (prefixStart > 0 && IsIdentifierCharacter(text[prefixStart - 1])) {
            --prefixStart;
        }

        request.prefix = text.substr(prefixStart, cursorOffset - prefixStart);
        return request;
    }

    std::optional<std::string> GetInsertedText(const std::string& before, const std::string& after) {
        if (after.size() < before.size()) {
            return std::nullopt;
        }

        std::size_t prefix = 0;
        while (prefix < before.size() && before[prefix] == after[prefix]) {
            ++prefix;
        }

        std::size_t beforeSuffix = before.size();
        std::size_t afterSuffix = after.size();
        while (beforeSuffix > prefix &&
               afterSuffix > prefix &&
               before[beforeSuffix - 1] == after[afterSuffix - 1]) {
            --beforeSuffix;
            --afterSuffix;
        }

        if (beforeSuffix != prefix) {
            return std::nullopt;
        }

        return after.substr(prefix, afterSuffix - prefix);
    }

    bool ShouldSuppressAutocompleteForInsertedText(const std::string& insertedText) {
        return std::any_of(insertedText.begin(), insertedText.end(), [](unsigned char value) {
            return std::isspace(value) != 0 || value == ';';
        });
    }

    bool IsPrimaryShortcutPressed(ImGuiKey key) {
        auto& io = ImGui::GetIO();
        const bool primaryModifier = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
        const bool shift = io.KeyShift;
        const bool alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;
        return primaryModifier && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(key));
    }

    bool IsCompletionShortcutPressed() {
        auto& io = ImGui::GetIO();
        const bool shift = io.KeyShift;
        const bool alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

        if (io.ConfigMacOSXBehaviors) {
            return io.KeySuper && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_J));
        }

        return io.KeyCtrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space));
    }

    ImVec2 CalculateCompletionPopupSize(std::size_t candidateCount) {
        const auto& style = ImGui::GetStyle();
        const float rowHeight = ImGui::GetTextLineHeightWithSpacing();
        const float visibleRows = static_cast<float>(std::min<std::size_t>(8, candidateCount));
        const float height = std::clamp(
            style.WindowPadding.y * 2.0f + visibleRows * rowHeight,
            CompletionPopupMinHeight,
            CompletionPopupMaxHeight
        );

        return ImVec2(CompletionPopupWidth, height);
    }

    ImVec2 CalculateCompletionPopupPosition(const TextEditor& editor, const ImVec2& popupSize) {
        const ImVec2 editorMin = editor.GetContentScreenMin();
        const ImVec2 editorMax = editor.GetContentScreenMax();
        const ImVec2 cursor = editor.GetCursorScreenPosition();

        const float maxX = std::max(editorMin.x, editorMax.x - popupSize.x);
        const float maxY = std::max(editorMin.y, editorMax.y - popupSize.y);

        float x = std::clamp(cursor.x, editorMin.x, maxX);
        float y = cursor.y + std::max(editor.GetLineHeight(), ImGui::GetTextLineHeight()) + CompletionPopupVerticalOffset;

        const float aboveY = cursor.y - popupSize.y - CompletionPopupVerticalOffset;
        if (y + popupSize.y > editorMax.y && aboveY >= editorMin.y) {
            y = aboveY;
        }

        y = std::clamp(y, editorMin.y, maxY);
        return ImVec2(x, y);
    }

    bool IsPointInsideRect(const ImVec2& point, const ImVec2& min, const ImVec2& max) {
        return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y;
    }
}

void CodeEditorWorkspace::OpenFile(const std::string& path) {
    for (auto& document : documents) {
        if (document.path == path) {
            activePath = path;
            document.requestSelection = true;
            return;
        }
    }

    documents.emplace_back();
    auto& document = documents.back();
    document.path = path;
    document.title = std::filesystem::path(path).filename().string();
    document.savedText = FileHelper::ReadAllText(path);
    document.editor.SetPalette(TextEditor::GetDarkPalette());
    document.editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
    document.editor.SetText(document.savedText);
    document.requestSelection = true;

    activePath = path;
    statusText = "Opened " + document.title;
}

void CodeEditorWorkspace::SetStatusText(std::string text) {
    statusText = std::move(text);
}

const std::string& CodeEditorWorkspace::StatusText() const {
    return statusText;
}

const std::string& CodeEditorWorkspace::ActivePath() const {
    return activePath;
}

bool CodeEditorWorkspace::SaveDocument(Document& document) {
    auto text = document.editor.GetText();
    if (!FileHelper::TryWriteAllText(document.path, text)) {
        statusText = "Failed to save " + document.title;
        return false;
    }

    document.savedText = std::move(text);
    document.isDirty = false;
    statusText = "Saved " + document.title;
    return true;
}

void CodeEditorWorkspace::CloseCompletion(Document& document) {
    document.completion = {};
}

void CodeEditorWorkspace::RequestCompletion(Document& document, CodeEditorAutocomplete& autocomplete) {
    const auto request = BuildCompletionRequest(document.editor);
    const auto requestId = autocomplete.QueueCompletion(document.path, document.editor);
    if (requestId == 0) {
        CloseCompletion(document);
        return;
    }

    const bool canKeepVisibleCandidates = document.completion.isOpen &&
                                          !document.completion.candidates.empty() &&
                                          request.cursorPosition.mLine == document.completion.cursorPosition.mLine &&
                                          request.prefix.starts_with(document.completion.prefix);

    document.completion.isOpen = true;
    document.completion.isLoading = true;
    document.completion.selectedIndex = 0;
    document.completion.requestId = requestId;
    document.completion.prefix = request.prefix;
    document.completion.cursorPosition = request.cursorPosition;
    if (!canKeepVisibleCandidates) {
        document.completion.candidates.clear();
    }
}

void CodeEditorWorkspace::ApplyCompletionResult(const CodeEditorAutocompleteResult& result) {
    for (auto& document : documents) {
        if (document.path != result.filePath) {
            continue;
        }

        if (document.completion.requestId != result.requestId) {
            return;
        }

        document.completion.isLoading = false;
        document.completion.prefix = result.prefix;
        document.completion.cursorPosition = result.cursorPosition;
        document.completion.candidates = result.candidates;
        document.completion.selectedIndex = 0;

        if (document.completion.candidates.empty()) {
            CloseCompletion(document);
        }

        return;
    }
}

void CodeEditorWorkspace::PumpCompletionResults(CodeEditorAutocomplete& autocomplete) {
    while (true) {
        auto result = autocomplete.TakeCompletedResult();
        if (!result.has_value()) {
            return;
        }

        ApplyCompletionResult(*result);
    }
}

void CodeEditorWorkspace::ApplySelectedCompletion(Document& document) {
    if (!document.completion.isOpen || document.completion.candidates.empty()) {
        CloseCompletion(document);
        return;
    }

    const auto request = BuildCompletionRequest(document.editor);
    const auto& candidate = document.completion.candidates[document.completion.selectedIndex];
    const auto originalText = document.editor.GetText();
    const auto cursorOffset = CursorOffsetFromCoordinates(originalText, request.cursorPosition);
    const bool shouldInsertOpeningParenthesis = candidate.appendOpeningParenthesis &&
                                                (cursorOffset >= originalText.size() || originalText[cursorOffset] != '(');

    if (!request.prefix.empty()) {
        document.editor.MoveLeft(static_cast<int>(request.prefix.size()), true, false);
        document.editor.Delete();
    }

    document.editor.InsertText(candidate.insertText.c_str());
    if (shouldInsertOpeningParenthesis) {
        document.editor.InsertText("(");
    }
    document.isDirty = document.editor.GetText() != document.savedText;
    statusText = "Inserted completion: " + candidate.insertText + (shouldInsertOpeningParenthesis ? "(" : "");
    CloseCompletion(document);
}

void CodeEditorWorkspace::DrawCompletionPopup(Document& document, const std::string& popupId) {
    const ImVec2 popupSize = CalculateCompletionPopupSize(document.completion.candidates.size());
    const ImVec2 popupPosition = CalculateCompletionPopupPosition(document.editor, popupSize);

    ImGui::SetNextWindowPos(popupPosition, ImGuiCond_Always);
    ImGui::SetNextWindowSize(popupSize, ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 6.0f));

    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                   ImGuiWindowFlags_NoDocking |
                                   ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoSavedSettings |
                                   ImGuiWindowFlags_NoFocusOnAppearing |
                                   ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin(popupId.c_str(), nullptr, flags);

    if (document.completion.isLoading && document.completion.candidates.empty()) {
        ImGui::TextUnformatted("Loading completions...");
    } else {
        for (int i = 0; i < static_cast<int>(document.completion.candidates.size()); ++i) {
            const auto& candidate = document.completion.candidates[i];
            std::string label = candidate.displayText;
            if (!candidate.returnType.empty()) {
                label += "    " + candidate.returnType;
            }

            if (i == document.completion.selectedIndex) {
                ImGui::SetScrollHereY();
            }

            if (ImGui::Selectable(label.c_str(), i == document.completion.selectedIndex)) {
                document.completion.selectedIndex = i;
                ApplySelectedCompletion(document);
                break;
            }
        }
    }

    const bool popupHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    ImGui::End();
    ImGui::PopStyleVar();

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !popupHovered) {
        const auto mousePosition = ImGui::GetMousePos();
        const bool insideEditor = IsPointInsideRect(mousePosition, document.editor.GetContentScreenMin(), document.editor.GetContentScreenMax());
        if (!insideEditor) {
            CloseCompletion(document);
        }
    }
}

void CodeEditorWorkspace::DrawStatusBar(const Document& document) const {
    const auto cursor = document.editor.GetCursorPosition();
    const char* primaryModifier = ImGui::GetIO().ConfigMacOSXBehaviors ? "Cmd" : "Ctrl";
    const char* completionShortcut = ImGui::GetIO().ConfigMacOSXBehaviors ? "Cmd+J Complete" : "Ctrl+Space Complete";

    ImGui::Separator();
    ImGui::Text("Line %d, Column %d", cursor.mLine + 1, cursor.mColumn + 1);
    ImGui::SameLine();
    ImGui::TextUnformatted(document.isDirty ? "Modified" : "Saved");
    ImGui::SameLine();
    ImGui::Text("%s+S Save", primaryModifier);
    ImGui::SameLine();
    ImGui::TextUnformatted(completionShortcut);
}

void CodeEditorWorkspace::DrawDocument(Document& document, int index, CodeEditorAutocomplete& autocomplete, ImFont* codeFont) {
    if (ImGui::Button("Save")) {
        SaveDocument(document);
    }

    ImGui::SameLine();
    if (ImGui::Button("Complete")) {
        RequestCompletion(document, autocomplete);
    }

    ImGui::SameLine();
    ImGui::TextWrapped("%s", document.path.c_str());

    ImGui::Separator();

    if (codeFont != nullptr) {
        ImGui::PushFont(codeFont);
    }

    const auto textBeforeEdit = document.editor.GetText();
    const float statusHeight = ImGui::GetTextLineHeightWithSpacing() + ImGui::GetStyle().ItemSpacing.y * 2.0f;
    const float editorHeight = std::max(120.0f, ImGui::GetContentRegionAvail().y - statusHeight);

    const bool completionConsumesKeyboard = document.completion.isOpen &&
                                            (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)) ||
                                             ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) ||
                                             ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)) ||
                                             ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab)) ||
                                             ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)));

    document.editor.SetHandleKeyboardInputs(!completionConsumesKeyboard);
    document.editor.Render(("##CodeEditor" + std::to_string(index)).c_str(), ImVec2(0.0f, editorHeight), true);
    const auto textAfterEdit = document.editor.GetText();
    document.isDirty = textAfterEdit != document.savedText;

    const bool editorFocused = document.editor.IsFocused();
    const bool textChanged = document.editor.IsTextChanged();
    const bool cursorChanged = document.editor.IsCursorPositionChanged();

    if (editorFocused && IsPrimaryShortcutPressed(ImGuiKey_S)) {
        SaveDocument(document);
    }

    if (editorFocused && textChanged) {
        const auto insertedText = GetInsertedText(textBeforeEdit, textAfterEdit);
        if (insertedText.has_value() && !insertedText->empty() && !ShouldSuppressAutocompleteForInsertedText(*insertedText)) {
            RequestCompletion(document, autocomplete);
        } else {
            CloseCompletion(document);
        }
    } else if (editorFocused && IsCompletionShortcutPressed()) {
        RequestCompletion(document, autocomplete);
    }

    if (document.completion.isOpen && cursorChanged && !textChanged && !completionConsumesKeyboard) {
        CloseCompletion(document);
    }

    if (document.completion.isOpen) {
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)) && !document.completion.candidates.empty()) {
            document.completion.selectedIndex = (document.completion.selectedIndex + 1) % static_cast<int>(document.completion.candidates.size());
        } else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) && !document.completion.candidates.empty()) {
            const int count = static_cast<int>(document.completion.candidates.size());
            document.completion.selectedIndex = (document.completion.selectedIndex - 1 + count) % count;
        } else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab))) {
            ApplySelectedCompletion(document);
        } else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
            CloseCompletion(document);
        }
    }

    if (document.completion.isOpen) {
        DrawCompletionPopup(document, "##CompletionPopup" + std::to_string(index));
    }

    DrawStatusBar(document);

    if (codeFont != nullptr) {
        ImGui::PopFont();
    }
}

void CodeEditorWorkspace::Draw(CodeEditorAutocomplete& autocomplete, ImFont* codeFont) {
    PumpCompletionResults(autocomplete);
    ImGui::Begin("Code Editor");

    if (documents.empty()) {
        ImGui::TextWrapped("Open a .cpp or .hpp file from the Project window to start editing.");
        if (!statusText.empty()) {
            ImGui::Separator();
            ImGui::TextWrapped("%s", statusText.c_str());
        }
        ImGui::End();
        return;
    }

    int closeDocumentIndex = -1;

    if (ImGui::BeginTabBar("CodeEditorTabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyResizeDown)) {
        for (int i = 0; i < static_cast<int>(documents.size()); ++i) {
            auto& document = documents[i];
            bool isOpen = true;
            std::string title = document.title;
            if (document.isDirty) {
                title += "*";
            }

            ImGui::PushID(document.path.c_str());
            const auto flags = document.requestSelection ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None;
            if (ImGui::BeginTabItem(title.c_str(), &isOpen, flags)) {
                activePath = document.path;
                document.requestSelection = false;
                DrawDocument(document, i, autocomplete, codeFont);
                ImGui::EndTabItem();
            }
            ImGui::PopID();

            if (!isOpen) {
                closeDocumentIndex = i;
            }
        }

        ImGui::EndTabBar();
    }

    if (closeDocumentIndex >= 0) {
        const auto closedPath = documents[closeDocumentIndex].path;
        documents.erase(documents.begin() + closeDocumentIndex);

        if (activePath == closedPath) {
            activePath = documents.empty() ? "" : documents.back().path;
        }
    }

    if (!statusText.empty()) {
        ImGui::Separator();
        ImGui::TextWrapped("%s", statusText.c_str());
    }

    ImGui::End();
}
