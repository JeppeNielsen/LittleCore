//
// Created by Jeppe Nielsen on 15/04/2024.
//

#include "MainState.hpp"
#include "imgui.h"
#include <string>
#include "misc/cpp/imgui_stdlib.h"
#include <iostream>
#include <fstream>
#include "Timer.hpp"

static std::string text;
static std::string text2;

MainState::MainState() :
moduleDefinitionsManager(moduleSettings),
moduleManager(engineContext),
cin("input.txt"),
cout("output.txt")
{

}

void MainState::Initialize() {

    // optional performance optimizations
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    std::cin.rdbuf(cin.rdbuf());
    std::cout.rdbuf(cout.rdbuf());

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/");

    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/Files/Vector2.cpp");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/Files");


    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui_demo.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui_draw.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui_tables.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui_widgets.cpp");

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/misc/cpp/");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/misc/cpp/imgui_stdlib.cpp");

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/misc/cpp/");

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/entt/src/");

    moduleSettings.libraryFolder = "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Cache/";

    auto& consoleDefinition = moduleDefinitionsManager.CreateFromMainFile("Console", "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/ConsoleWindow/Console.cpp");

    LittleCore::Timer timer;
    timer.Start();
    consoleDefinition.Build();
    float duration = timer.Stop();
    std::cout << "Compilation took: " << std::to_string(duration) << "\n";

    moduleManager.AddModule("Console", consoleDefinition);

    moduleManager.GetModules()["Console"]->Load();

    gui.Initialize(mainWindow, [this, &consoleDefinition]() {
        ImGui::DockSpaceOverViewport();
        ImGui::Begin("Window");

        ImGui::InputText("Name", &text);
        ImGui::SameLine(0,1);
        ImGui::InputText("Email", &text2);

        if (ImGui::Button("Recompile")) {

            moduleManager.GetModules()["Console"]->Unload();
            auto result = consoleDefinition.Build();

            if (!result.errors.empty()) {
                errors = result.errors[1];
            }

            moduleManager.GetModules()["Console"]->Load();

        }

        ImGui::InputTextMultiline("Errors:", &errors);

        ImGui::End();

        moduleManager.GetModules()["Console"]->OnGui(engineContext.imGuiContext);
    });

    gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Assets/Fonts/LucidaG.ttf", 14);
    engineContext.imGuiContext = ImGui::GetCurrentContext();
}

void MainState::Update(float dt) {
    //std::cout << "Text: "<< text<< std::endl;
    moduleManager.GetModules()["Console"]->Update(dt);
}

void MainState::Render() {

    ImGui::SetCurrentContext(engineContext.imGuiContext);
    gui.Render();
    moduleManager.GetModules()["Console"]->Render();
}

void MainState::HandleEvent(void *event) {
    gui.HandleEvent(event);
}
