//
// Created by Jeppe Nielsen on 30/11/2024.
//

#include <imgui.h>
#include "misc/cpp/imgui_stdlib.h"
#include "CompilerWindow.hpp"

CompilerWindow::CompilerWindow(ProjectCompiler &compiler) : compiler(compiler) {

}

void CompilerWindow::DrawGui() {
    ImGui::Begin("Compilation", &open, ImGuiWindowFlags_HorizontalScrollbar);
    if (!compiler.IsCompiling()) {
        if (ImGui::Button("Compile")) {
            Compile();
        }
    } else {
        ImGui::Text("Compiling...");
    }

    DrawErrors();
    ImGui::End();
}

void CompilerWindow::Compile() {
    compiler.Compile();
}

void CompilerWindow::DrawErrors() {
    std::string errorReport;

    for(auto e : compiler.Result().errors) {
        errorReport += e + "\n";
    }
    ImGui::PushItemWidth(-1);
    ImGui::InputTextMultiline("##", &errorReport, ImVec2(-1, -1));
    ImGui::PopItemWidth();
}
