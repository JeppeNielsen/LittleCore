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

MainState::MainState() :
moduleDefinitionsManager(moduleSettings),
moduleManager(engineContext),
cin("input.txt"),
cout("output.txt")
{

}

void MainState::Initialize() {

    engineContext.registryCollection = &registyCollection;

    // optional performance optimizations
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    std::cin.rdbuf(cin.rdbuf());
    //std::cout.rdbuf(cout.rdbuf());

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/");

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

    projectSettings.rootPath = "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/TestProject";

    projectSettings.ModifyModules(moduleSettings.context, moduleDefinitionsManager);

    //auto& consoleDefinition = moduleDefinitionsManager.CreateFromMainFile("Console", "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/ConsoleWindow/Console.cpp");

    for(auto& definition : moduleDefinitionsManager.Definitions()) {
        moduleManager.AddModule(definition.first, *definition.second.get());
    }

    //Compile();

    for(auto& module : moduleManager.GetModules()) {
        module.second->Load();
    }

    gui.Initialize(mainWindow, [this]() {
        ImGui::DockSpaceOverViewport();
        ImGui::Begin("Compilation");

            if (ImGui::Button("Recompile")) {
                Compile();
            }

        ImGui::End();

        ImGui::Begin("Errors");
            std::string errorReport;

            for (auto error : errors) {
                errorReport += error;
            }

            ImGui::InputTextMultiline("Errors:", &errorReport);
        ImGui::End();


        for(auto& module : moduleManager.GetModules()) {
            module.second->OnGui(engineContext.imGuiContext);
        }
    });

    gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Assets/Fonts/LucidaG.ttf", 14);
    engineContext.imGuiContext = ImGui::GetCurrentContext();

}

void MainState::Update(float dt) {
    //std::cout << "Text: "<< text<< std::endl;
    for(auto& module : moduleManager.GetModules()) {
        module.second->Update(dt);
    }
}

void MainState::Render() {

    ImGui::SetCurrentContext(engineContext.imGuiContext);
    gui.Render();

    for(auto& module : moduleManager.GetModules()) {
        module.second->Render();
    }
}

void MainState::HandleEvent(void *event) {
    gui.HandleEvent(event);
}

void MainState::Compile() {

    for(auto& module : moduleManager.GetModules()) {
        module.second->Unload();
    }

    errors.clear();

    for(auto& definition : moduleDefinitionsManager.Definitions()) {
        LittleCore::Timer timer;
        timer.Start();
        auto result = definition.second->Build();

        float duration = timer.Stop();
        std::cout << "Compilation took for module: "<<definition.first << ", took = " << std::to_string(duration) << "\n";

        for(auto error : result.errors) {
            errors.push_back(error);
        }
    }

    for(auto& module : moduleManager.GetModules()) {
        module.second->Load();
    }

}
