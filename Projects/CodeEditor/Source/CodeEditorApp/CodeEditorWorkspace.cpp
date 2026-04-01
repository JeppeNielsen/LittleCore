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
    constexpr float SignaturePopupWidth = 360.0f;
    constexpr float SignaturePopupMinHeight = 52.0f;
    constexpr float SignaturePopupMaxHeight = 220.0f;
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

    TextEditor::Coordinates CoordinatesFromOffset(const std::string& text, std::size_t offset) {
        TextEditor::Coordinates coordinates;
        offset = std::min(offset, text.size());

        for (std::size_t i = 0; i < offset; ++i) {
            if (text[i] == '\n') {
                ++coordinates.mLine;
                coordinates.mColumn = 0;
            } else {
                ++coordinates.mColumn;
            }
        }

        return coordinates;
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
            return std::isspace(value) != 0 || value == ';' || value == '{' || value == '}' || value == ')';
        });
    }

    bool ContainsCharacter(const std::string& text, char value) {
        return text.find(value) != std::string::npos;
    }

    bool IsEscapedCharacter(const std::string& text, std::size_t index) {
        if (index == 0 || index > text.size()) {
            return false;
        }

        std::size_t backslashCount = 0;
        std::size_t currentIndex = index;
        while (currentIndex > 0 && text[currentIndex - 1] == '\\') {
            --currentIndex;
            ++backslashCount;
        }

        return (backslashCount % 2) != 0;
    }

    struct CallableNameRange {
        std::size_t start = 0;
        std::size_t end = 0;
        std::string name;
    };

    std::optional<CallableNameRange> FindCallableNameRange(const std::string& text, std::size_t openParenthesisOffset) {
        if (openParenthesisOffset >= text.size() || text[openParenthesisOffset] != '(') {
            return std::nullopt;
        }

        std::size_t position = openParenthesisOffset;
        while (position > 0 && std::isspace(static_cast<unsigned char>(text[position - 1])) != 0) {
            --position;
        }

        if (position == 0) {
            return std::nullopt;
        }

        if (text[position - 1] == '>') {
            int depth = 0;
            bool foundOpeningAngle = false;

            while (position > 0) {
                const char value = text[position - 1];
                --position;

                if (value == '>') {
                    ++depth;
                } else if (value == '<') {
                    --depth;
                    if (depth == 0) {
                        foundOpeningAngle = true;
                        break;
                    }
                }
            }

            if (!foundOpeningAngle) {
                return std::nullopt;
            }

            while (position > 0 && std::isspace(static_cast<unsigned char>(text[position - 1])) != 0) {
                --position;
            }
        }

        const std::size_t end = position;
        while (position > 0 && IsIdentifierCharacter(text[position - 1])) {
            --position;
        }

        if (position > 0 && position < end && text[position - 1] == '~') {
            --position;
        }

        if (position == end) {
            return std::nullopt;
        }

        return CallableNameRange{
                .start = position,
                .end = end,
                .name = text.substr(position, end - position)
        };
    }

    std::optional<std::size_t> FindActiveCallableOpenParenthesisOffset(const std::string& text, std::size_t cursorOffset) {
        cursorOffset = std::min(cursorOffset, text.size());

        std::vector<std::size_t> openParentheses;
        bool inString = false;
        char stringDelimiter = '\0';
        bool inLineComment = false;
        bool inBlockComment = false;

        for (std::size_t i = 0; i < cursorOffset; ++i) {
            const char value = text[i];
            const char nextValue = i + 1 < cursorOffset ? text[i + 1] : '\0';

            if (inLineComment) {
                if (value == '\n') {
                    inLineComment = false;
                }
                continue;
            }

            if (inBlockComment) {
                if (value == '*' && nextValue == '/') {
                    inBlockComment = false;
                    ++i;
                }
                continue;
            }

            if (inString) {
                if (value == stringDelimiter && !IsEscapedCharacter(text, i)) {
                    inString = false;
                }
                continue;
            }

            if (value == '/' && nextValue == '/') {
                inLineComment = true;
                ++i;
                continue;
            }

            if (value == '/' && nextValue == '*') {
                inBlockComment = true;
                ++i;
                continue;
            }

            if (value == '"' || value == '\'') {
                inString = true;
                stringDelimiter = value;
                continue;
            }

            if (value == '(') {
                openParentheses.push_back(i);
                continue;
            }

            if (value == ')' && !openParentheses.empty()) {
                openParentheses.pop_back();
            }
        }

        for (auto it = openParentheses.rbegin(); it != openParentheses.rend(); ++it) {
            if (FindCallableNameRange(text, *it).has_value()) {
                return *it;
            }
        }

        return std::nullopt;
    }

    std::optional<int> ComputeActiveParameterIndex(const std::string& text,
                                                   std::size_t openParenthesisOffset,
                                                   std::size_t cursorOffset) {
        if (openParenthesisOffset >= text.size() ||
            text[openParenthesisOffset] != '(' ||
            cursorOffset < openParenthesisOffset + 1 ||
            cursorOffset > text.size()) {
            return std::nullopt;
        }

        int activeParameterIndex = 0;
        int nestedParentheses = 0;
        int nestedBrackets = 0;
        int nestedBraces = 0;
        bool inString = false;
        char stringDelimiter = '\0';

        for (std::size_t i = openParenthesisOffset + 1; i < cursorOffset; ++i) {
            const char value = text[i];

            if (inString) {
                if (value == stringDelimiter && (i == 0 || text[i - 1] != '\\')) {
                    inString = false;
                }
                continue;
            }

            if (value == '"' || value == '\'') {
                inString = true;
                stringDelimiter = value;
                continue;
            }

            switch (value) {
                case '(':
                    ++nestedParentheses;
                    break;
                case ')':
                    if (nestedParentheses == 0) {
                        return std::nullopt;
                    }
                    --nestedParentheses;
                    break;
                case '[':
                    ++nestedBrackets;
                    break;
                case ']':
                    nestedBrackets = std::max(0, nestedBrackets - 1);
                    break;
                case '{':
                    ++nestedBraces;
                    break;
                case '}':
                    nestedBraces = std::max(0, nestedBraces - 1);
                    break;
                case ',':
                    if (nestedParentheses == 0 && nestedBrackets == 0 && nestedBraces == 0) {
                        ++activeParameterIndex;
                    }
                    break;
                default:
                    break;
            }
        }

        return activeParameterIndex;
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
        const ImVec2 cursorTop = editor.GetCursorScreenPosition();
        const ImVec2 cursorBottom = editor.GetCursorScreenBottom();

        const float maxX = std::max(editorMin.x, editorMax.x - popupSize.x);
        const float maxY = std::max(editorMin.y, editorMax.y - popupSize.y);

        float x = std::clamp(cursorTop.x, editorMin.x, maxX);
        float y = cursorBottom.y + CompletionPopupVerticalOffset;

        const float aboveY = cursorTop.y - popupSize.y - CompletionPopupVerticalOffset;
        if (y + popupSize.y > editorMax.y && aboveY >= editorMin.y) {
            y = aboveY;
        }

        y = std::clamp(y, editorMin.y, maxY);
        return ImVec2(x, y);
    }

    ImVec2 CalculateSignaturePopupSize(std::size_t parameterCount) {
        const auto& style = ImGui::GetStyle();
        const float rowHeight = ImGui::GetTextLineHeightWithSpacing();
        const float lineCount = 1.0f + static_cast<float>(std::max<std::size_t>(1, parameterCount));
        const float height = std::clamp(
            style.WindowPadding.y * 2.0f + lineCount * rowHeight,
            SignaturePopupMinHeight,
            SignaturePopupMaxHeight
        );

        return ImVec2(SignaturePopupWidth, height);
    }

    ImVec2 CalculateSignaturePopupPosition(const TextEditor& editor, const ImVec2& popupSize) {
        const ImVec2 editorMin = editor.GetContentScreenMin();
        const ImVec2 editorMax = editor.GetContentScreenMax();
        const ImVec2 cursorTop = editor.GetCursorScreenPosition();
        const ImVec2 cursorBottom = editor.GetCursorScreenBottom();

        const float maxX = std::max(editorMin.x, editorMax.x - popupSize.x);
        const float maxY = std::max(editorMin.y, editorMax.y - popupSize.y);

        float x = std::clamp(cursorTop.x, editorMin.x, maxX);
        float y = cursorTop.y - popupSize.y - CompletionPopupVerticalOffset;

        if (y < editorMin.y) {
            y = std::min(cursorBottom.y + CompletionPopupVerticalOffset, maxY);
        }

        y = std::clamp(y, editorMin.y, maxY);
        return ImVec2(x, y);
    }

    bool IsPointInsideRect(const ImVec2& point, const ImVec2& min, const ImVec2& max) {
        return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y;
    }
}

void CodeEditorWorkspace::OpenFile(const std::string& path) {
    auto& document = OpenOrCreateDocument(path);
    activePath = path;
    document.requestSelection = true;
}

void CodeEditorWorkspace::OpenFileAtLine(const std::string& path, int line) {
    auto& document = OpenOrCreateDocument(path);
    document.pendingCursorPosition = TextEditor::Coordinates(std::max(0, line - 1), 0);
    document.requestSelection = true;
    activePath = path;
    statusText = "Opened " + document.title + " at line " + std::to_string(std::max(1, line));
}

std::vector<SourceBreakpoint> CodeEditorWorkspace::SourceBreakpoints() const {
    std::vector<SourceBreakpoint> breakpoints;
    for (const auto& [path, lines] : fileBreakpoints) {
        for (const int line : lines) {
            if (line > 0) {
                breakpoints.push_back({path, line});
            }
        }
    }

    std::sort(breakpoints.begin(), breakpoints.end(), [](const SourceBreakpoint& left, const SourceBreakpoint& right) {
        if (left.filePath != right.filePath) {
            return left.filePath < right.filePath;
        }
        return left.line < right.line;
    });
    return breakpoints;
}

bool CodeEditorWorkspace::ConsumeBreakpointsChanged() {
    const bool changed = breakpointsChanged;
    breakpointsChanged = false;
    return changed;
}

CodeEditorWorkspace::Document& CodeEditorWorkspace::OpenOrCreateDocument(const std::string& path) {
    for (auto& document : documents) {
        if (document.path == path) {
            return document;
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
    auto breakpointsIt = fileBreakpoints.find(path);
    if (breakpointsIt != fileBreakpoints.end()) {
        document.editor.SetBreakpoints(breakpointsIt->second);
    }

    statusText = "Opened " + document.title;
    return document;
}

void CodeEditorWorkspace::ToggleBreakpoint(Document& document, int line) {
    if (line <= 0) {
        return;
    }

    auto breakpoints = document.editor.GetBreakpoints();
    const auto breakpointIt = breakpoints.find(line);
    if (breakpointIt == breakpoints.end()) {
        breakpoints.insert(line);
        statusText = "Added breakpoint at " + document.title + ":" + std::to_string(line);
    } else {
        breakpoints.erase(breakpointIt);
        statusText = "Removed breakpoint at " + document.title + ":" + std::to_string(line);
    }

    document.editor.SetBreakpoints(breakpoints);
    SyncBreakpointsFromEditor(document);
}

void CodeEditorWorkspace::SyncBreakpointsFromEditor(const Document& document) {
    const auto& editorBreakpoints = document.editor.GetBreakpoints();
    auto breakpointsIt = fileBreakpoints.find(document.path);
    if (editorBreakpoints.empty()) {
        if (breakpointsIt != fileBreakpoints.end()) {
            fileBreakpoints.erase(breakpointsIt);
            breakpointsChanged = true;
        }
        return;
    }

    if (breakpointsIt == fileBreakpoints.end() || breakpointsIt->second != editorBreakpoints) {
        fileBreakpoints[document.path] = editorBreakpoints;
        breakpointsChanged = true;
    }
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

void CodeEditorWorkspace::CloseSignatureHelp(Document& document) {
    document.signatureHelp = {};
}

void CodeEditorWorkspace::OpenSignatureHelp(Document& document,
                                            const CodeEditorCompletionCandidate& candidate,
                                            std::size_t openParenthesisOffset) {
    if (candidate.parameters.empty()) {
        CloseSignatureHelp(document);
        return;
    }

    document.signatureHelp.isOpen = true;
    document.signatureHelp.functionName = candidate.insertText;
    document.signatureHelp.parameters = candidate.parameters;
    document.signatureHelp.openParenthesisOffset = openParenthesisOffset;
    document.signatureHelp.activeParameterIndex = 0;
    document.signatureHelp.lookupFailed = false;
}

void CodeEditorWorkspace::RequestSignatureHelp(Document& document,
                                               CodeEditorAutocomplete& autocomplete,
                                               std::size_t openParenthesisOffset) {
    const auto requestId = autocomplete.QueueSignatureHelp(document.path, document.editor, openParenthesisOffset);
    if (requestId == 0) {
        CloseSignatureHelp(document);
        return;
    }

    document.signatureHelp = {};
    document.signatureHelp.requestId = requestId;
    document.signatureHelp.openParenthesisOffset = openParenthesisOffset;
    document.signatureHelp.lookupFailed = false;
}

void CodeEditorWorkspace::SyncSignatureHelp(Document& document,
                                            CodeEditorAutocomplete& autocomplete,
                                            bool allowOpen) {
    const auto text = document.editor.GetText();
    const auto cursor = document.editor.GetCursorPosition();
    const auto cursorOffset = CursorOffsetFromCoordinates(text, cursor);
    const auto openParenthesisOffset = FindActiveCallableOpenParenthesisOffset(text, cursorOffset);

    if (!openParenthesisOffset.has_value()) {
        CloseSignatureHelp(document);
        return;
    }

    if (document.signatureHelp.openParenthesisOffset != *openParenthesisOffset) {
        if (allowOpen || document.signatureHelp.isOpen || document.signatureHelp.requestId != 0) {
            RequestSignatureHelp(document, autocomplete, *openParenthesisOffset);
        }
        return;
    }

    if (document.signatureHelp.isOpen) {
        UpdateSignatureHelp(document);
        return;
    }

    if (document.signatureHelp.requestId != 0) {
        return;
    }

    if (document.signatureHelp.lookupFailed && !allowOpen) {
        return;
    }

    if (allowOpen) {
        RequestSignatureHelp(document, autocomplete, *openParenthesisOffset);
    }
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
                                          request.cursorPosition.mLine == document.completion.cursorPosition.mLine;

    document.completion.isOpen = canKeepVisibleCandidates;
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
        } else {
            document.completion.isOpen = true;
        }

        return;
    }
}

void CodeEditorWorkspace::ApplySignatureHelpResult(const CodeEditorSignatureHelpResult& result) {
    for (auto& document : documents) {
        if (document.path != result.filePath) {
            continue;
        }

        if (document.signatureHelp.requestId != result.requestId) {
            return;
        }

        if (result.parameters.empty()) {
            document.signatureHelp = {};
            document.signatureHelp.openParenthesisOffset = result.openParenthesisOffset;
            document.signatureHelp.lookupFailed = true;
            return;
        }

        document.signatureHelp.isOpen = true;
        document.signatureHelp.requestId = result.requestId;
        document.signatureHelp.functionName = result.functionName;
        document.signatureHelp.parameters = result.parameters;
        document.signatureHelp.openParenthesisOffset = result.openParenthesisOffset;
        document.signatureHelp.activeParameterIndex = 0;
        document.signatureHelp.lookupFailed = false;
        UpdateSignatureHelp(document);
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

void CodeEditorWorkspace::PumpSignatureHelpResults(CodeEditorAutocomplete& autocomplete) {
    while (true) {
        auto result = autocomplete.TakeCompletedSignatureHelpResult();
        if (!result.has_value()) {
            return;
        }

        ApplySignatureHelpResult(*result);
    }
}

void CodeEditorWorkspace::ApplySelectedCompletion(Document& document) {
    if (!document.completion.isOpen || document.completion.candidates.empty()) {
        CloseCompletion(document);
        return;
    }

    const auto completionCursorPosition = document.completion.cursorPosition;
    const auto completionPrefix = document.completion.prefix;
    const auto& candidate = document.completion.candidates[document.completion.selectedIndex];
    const auto originalText = document.editor.GetText();
    const auto liveCursorOffset = CursorOffsetFromCoordinates(originalText, document.editor.GetCursorPosition());
    const auto completionCursorOffset = CursorOffsetFromCoordinates(originalText, completionCursorPosition);
    const auto cursorOffset = std::max(liveCursorOffset, completionCursorOffset);
    const bool shouldInsertOpeningParenthesis = candidate.appendOpeningParenthesis &&
                                                (cursorOffset >= originalText.size() || originalText[cursorOffset] != '(');
    const bool shouldInsertEmptyCall = shouldInsertOpeningParenthesis && candidate.parameters.empty();
    std::optional<std::size_t> signatureOpenParenthesisOffset;
    std::string insertedSuffix;

    if (!completionPrefix.empty()) {
        std::size_t replacementStartOffset = cursorOffset;
        while (replacementStartOffset > 0 && IsIdentifierCharacter(originalText[replacementStartOffset - 1])) {
            --replacementStartOffset;
        }

        document.editor.SetSelection(
                CoordinatesFromOffset(originalText, replacementStartOffset),
                CoordinatesFromOffset(originalText, cursorOffset));
        document.editor.Delete();
    } else {
        document.editor.SetCursorPosition(CoordinatesFromOffset(originalText, cursorOffset));
    }

    document.editor.InsertText(candidate.insertText.c_str());
    if (shouldInsertEmptyCall) {
        document.editor.InsertText("()");
        insertedSuffix = "()";
    } else if (shouldInsertOpeningParenthesis) {
        document.editor.InsertText("(");
        insertedSuffix = "(";
        const auto updatedText = document.editor.GetText();
        const auto updatedCursor = document.editor.GetCursorPosition();
        const auto updatedCursorOffset = CursorOffsetFromCoordinates(updatedText, updatedCursor);
        if (updatedCursorOffset > 0) {
            signatureOpenParenthesisOffset = updatedCursorOffset - 1;
        }
    } else if (candidate.isType) {
        document.editor.InsertText(" ");
        insertedSuffix = " ";
    }
    document.isDirty = document.editor.GetText() != document.savedText;
    statusText = "Inserted completion: " + candidate.insertText + insertedSuffix;
    if (signatureOpenParenthesisOffset.has_value()) {
        OpenSignatureHelp(document, candidate, *signatureOpenParenthesisOffset);
    } else {
        CloseSignatureHelp(document);
    }
    CloseCompletion(document);
}

void CodeEditorWorkspace::UpdateSignatureHelp(Document& document) {
    if (!document.signatureHelp.isOpen) {
        return;
    }

    const auto text = document.editor.GetText();
    const auto cursor = document.editor.GetCursorPosition();
    const auto cursorOffset = CursorOffsetFromCoordinates(text, cursor);
    const auto activeParameterIndex = ComputeActiveParameterIndex(
            text,
            document.signatureHelp.openParenthesisOffset,
            cursorOffset);

    if (!activeParameterIndex.has_value()) {
        CloseSignatureHelp(document);
        return;
    }

    if (document.signatureHelp.parameters.empty()) {
        CloseSignatureHelp(document);
        return;
    }

    document.signatureHelp.activeParameterIndex = std::clamp(
            *activeParameterIndex,
            0,
            static_cast<int>(document.signatureHelp.parameters.size()) - 1);
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

void CodeEditorWorkspace::DrawSignatureHelpPopup(const Document& document, const std::string& popupId) const {
    if (!document.signatureHelp.isOpen) {
        return;
    }

    const ImVec2 popupSize = CalculateSignaturePopupSize(document.signatureHelp.parameters.size());
    const ImVec2 popupPosition = CalculateSignaturePopupPosition(document.editor, popupSize);

    ImGui::SetNextWindowPos(popupPosition, ImGuiCond_Always);
    ImGui::SetNextWindowSize(popupSize, ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));

    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                   ImGuiWindowFlags_NoDocking |
                                   ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoSavedSettings |
                                   ImGuiWindowFlags_NoFocusOnAppearing |
                                   ImGuiWindowFlags_NoNavFocus |
                                   ImGuiWindowFlags_NoInputs;

    ImGui::Begin(popupId.c_str(), nullptr, flags);
    ImGui::Text("%s(", document.signatureHelp.functionName.c_str());
    ImGui::Separator();

    for (int i = 0; i < static_cast<int>(document.signatureHelp.parameters.size()); ++i) {
        const bool isActive = i == document.signatureHelp.activeParameterIndex;
        if (isActive) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_PlotHistogram));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
        }

        ImGui::Text("%s%s", isActive ? "> " : "  ", document.signatureHelp.parameters[i].c_str());
        ImGui::PopStyleColor();
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void CodeEditorWorkspace::DrawStatusBar(const Document& document) const {
    const auto cursor = document.editor.GetCursorPosition();
    const char* primaryModifier = ImGui::GetIO().ConfigMacOSXBehaviors ? "Cmd" : "Ctrl";
    const char* completionShortcut = ImGui::GetIO().ConfigMacOSXBehaviors ? "Cmd+J Complete" : "Ctrl+Space Complete";
    const int breakpointCount = static_cast<int>(document.editor.GetBreakpoints().size());

    ImGui::Separator();
    ImGui::Text("Line %d, Column %d", cursor.mLine + 1, cursor.mColumn + 1);
    ImGui::SameLine();
    ImGui::TextUnformatted(document.isDirty ? "Modified" : "Saved");
    ImGui::SameLine();
    ImGui::Text("%s+S Save", primaryModifier);
    ImGui::SameLine();
    ImGui::TextUnformatted(completionShortcut);
    ImGui::SameLine();
    ImGui::Text("F9 Breakpoint");
    ImGui::SameLine();
    ImGui::Text("Breakpoints %d", breakpointCount);
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
    if (ImGui::Button("Toggle Breakpoint")) {
        ToggleBreakpoint(document, document.editor.GetCursorPosition().mLine + 1);
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

    if (document.pendingCursorPosition.has_value()) {
        document.editor.SetCursorPosition(*document.pendingCursorPosition);
        document.pendingCursorPosition.reset();
    }

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
    const int toggledBreakpointLine = document.editor.ConsumeBreakpointToggleLine();
    if (toggledBreakpointLine > 0) {
        ToggleBreakpoint(document, toggledBreakpointLine);
    }
    SyncBreakpointsFromEditor(document);

    const bool editorFocused = document.editor.IsFocused();
    const bool textChanged = document.editor.IsTextChanged();
    const bool cursorChanged = document.editor.IsCursorPositionChanged();

    if (editorFocused && IsPrimaryShortcutPressed(ImGuiKey_S)) {
        SaveDocument(document);
    }

    if (editorFocused && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F9))) {
        ToggleBreakpoint(document, document.editor.GetCursorPosition().mLine + 1);
    }

    if (editorFocused && textChanged) {
        const auto insertedText = GetInsertedText(textBeforeEdit, textAfterEdit);
        if (insertedText.has_value() && !insertedText->empty()) {
            if (ContainsCharacter(*insertedText, '(')) {
                CloseCompletion(document);
            } else if (!ShouldSuppressAutocompleteForInsertedText(*insertedText)) {
                RequestCompletion(document, autocomplete);
            } else {
                CloseCompletion(document);
            }
        } else {
            CloseCompletion(document);
        }
    } else if (editorFocused && IsCompletionShortcutPressed()) {
        RequestCompletion(document, autocomplete);
    }

    if (document.completion.isOpen && cursorChanged && !textChanged && !completionConsumesKeyboard) {
        CloseCompletion(document);
    }

    const bool shouldAllowOpeningSignatureHelp = editorFocused && textChanged;
    if (editorFocused || document.signatureHelp.isOpen || document.signatureHelp.requestId != 0) {
        SyncSignatureHelp(document, autocomplete, shouldAllowOpeningSignatureHelp);
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

    if (document.signatureHelp.isOpen) {
        DrawSignatureHelpPopup(document, "##SignatureHelpPopup" + std::to_string(index));
    }

    DrawStatusBar(document);

    if (codeFont != nullptr) {
        ImGui::PopFont();
    }
}

void CodeEditorWorkspace::Draw(CodeEditorAutocomplete& autocomplete, ImFont* codeFont) {
    PumpCompletionResults(autocomplete);
    PumpSignatureHelpResults(autocomplete);
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
