//
// Created by Codex on 02/04/2026.
//

#include "CodeEditorBreakpointOverview.hpp"
#include "CodeEditorPathUtils.hpp"
#include "FileHelper.hpp"
#include "imgui.h"
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <string>

namespace {
    std::vector<std::string> SplitLines(const std::string& text) {
        std::vector<std::string> lines;
        std::stringstream stream(text);
        std::string line;
        while (std::getline(stream, line)) {
            lines.push_back(line);
        }

        if (!text.empty() && text.back() == '\n') {
            lines.emplace_back();
        }

        return lines;
    }

    std::string NormalizePreview(std::string line) {
        std::string result;
        result.reserve(line.size());
        for (const char value : line) {
            if (value == '\t') {
                result += "    ";
            } else {
                result += value;
            }
        }

        if (result.empty()) {
            return "<empty line>";
        }

        return result;
    }
}

const std::string& CodeEditorBreakpointOverview::GetLinePreview(const std::string& filePath, int line) const {
    static const std::string emptyPreview;

    if (line <= 0) {
        return emptyPreview;
    }

    RefreshCachedFile(filePath);

    const auto cacheIt = cachedFileLines.find(filePath);
    if (cacheIt == cachedFileLines.end()) {
        return emptyPreview;
    }

    const auto& lines = cacheIt->second.lines;
    const std::size_t lineIndex = static_cast<std::size_t>(line - 1);
    if (lineIndex >= lines.size()) {
        return emptyPreview;
    }

    return lines[lineIndex];
}

void CodeEditorBreakpointOverview::RefreshCachedFile(const std::string& filePath) const {
    std::error_code errorCode;
    const auto lastWriteTime = std::filesystem::last_write_time(filePath, errorCode);

    auto& cache = cachedFileLines[filePath];
    if (!errorCode && cache.hasTimestamp && cache.lastWriteTime == lastWriteTime) {
        return;
    }

    cache.lines = SplitLines(LittleCore::FileHelper::ReadAllText(filePath));
    cache.lastWriteTime = lastWriteTime;
    cache.hasTimestamp = !errorCode;
}

void CodeEditorBreakpointOverview::SetDisplayRootPath(std::string rootPath) {
    displayRootPath = std::move(rootPath);
}

CodeEditorBreakpointOverview::DrawResult CodeEditorBreakpointOverview::Draw(const std::vector<SourceBreakpoint>& breakpoints,
                                                                            const std::string& activePath) const {
    DrawResult result;

    ImGui::Begin("Breakpoints");
    ImGui::Text("Total: %d", static_cast<int>(breakpoints.size()));

    if (breakpoints.empty()) {
        ImGui::TextWrapped("No breakpoints are currently set.");
        ImGui::End();
        return result;
    }

    const auto& style = ImGui::GetStyle();
    auto* drawList = ImGui::GetWindowDrawList();

    for (const auto& breakpoint : breakpoints) {
        ImGui::PushID((breakpoint.filePath + ":" + std::to_string(breakpoint.line)).c_str());

        const float rowWidth = ImGui::GetContentRegionAvail().x;
        const ImVec2 rowMin = ImGui::GetCursorScreenPos();
        const auto fileName = std::filesystem::path(breakpoint.filePath).filename().string();
        const auto linePreview = NormalizePreview(GetLinePreview(breakpoint.filePath, breakpoint.line));

        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(0.86f, 0.28f, 0.28f, 1.0f), "Line %d  %s", breakpoint.line, fileName.c_str());
        ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + std::max(80.0f, rowWidth - style.FramePadding.x * 2.0f));
        const auto displayPath = CodeEditorPathUtils::MakeDisplayPath(breakpoint.filePath, displayRootPath);
        if (breakpoint.filePath == activePath) {
            ImGui::TextColored(ImVec4(0.95f, 0.75f, 0.75f, 1.0f), "%s", displayPath.c_str());
        } else {
            ImGui::TextDisabled("%s", displayPath.c_str());
        }
        ImGui::TextWrapped("%s", linePreview.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndGroup();

        const ImVec2 contentMin = ImGui::GetItemRectMin();
        const ImVec2 contentMax = ImGui::GetItemRectMax();
        const ImVec2 panelMin(contentMin.x - style.FramePadding.x, contentMin.y - style.FramePadding.y);
        const ImVec2 panelMax(rowMin.x + rowWidth, contentMax.y + style.FramePadding.y);
        const bool hovered = ImGui::IsMouseHoveringRect(panelMin, panelMax);
        const bool isActiveFile = breakpoint.filePath == activePath;

        if (hovered) {
            drawList->AddRect(panelMin, panelMax, ImGui::GetColorU32(ImVec4(0.86f, 0.28f, 0.28f, 0.85f)), 6.0f, 0, 1.5f);
        } else if (isActiveFile) {
            drawList->AddRect(panelMin, panelMax, ImGui::GetColorU32(ImVec4(0.86f, 0.28f, 0.28f, 0.45f)), 6.0f, 0, 1.0f);
        } else {
            drawList->AddRect(panelMin, panelMax, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.10f)), 6.0f, 0, 1.0f);
        }

        if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            result.openedPath = breakpoint.filePath;
            result.openedLine = breakpoint.line;
        }

        ImGui::Dummy(ImVec2(0.0f, style.ItemSpacing.y));
        ImGui::PopID();
    }

    ImGui::End();
    return result;
}
