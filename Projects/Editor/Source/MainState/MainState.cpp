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


            if (!isCompiling) {
                if (ImGui::Button("Recompile")) {

                    Compile();
                }
            } else {
                ImGui::Text("Compiling...");
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
    taskRunner.processTasks();

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



    errors.clear();

    compilationTimer.Start();
    isCompiling = true;

    taskRunner.runAsyncTask([this]{

       /* float expensive = 0;
        for (int i = 0; i < 100000000; ++i) {
            expensive += sin(i / cos(i));
        }
        */

        for(auto& definition : moduleDefinitionsManager.Definitions()) {
            auto result = definition.second->Build();

            for(auto error : result.errors) {
                errorsFromCompilation.push_back(error);
            }
        }
    }, [this] {


        float duration = compilationTimer.Stop();
        std::cout << "Compilation took = " << std::to_string(duration) << "\n";
        isCompiling = false;

        errorsFromCompilation.push_back("Compilation took = " + std::to_string(duration) + "\n");
        errors = errorsFromCompilation;
        errorsFromCompilation.clear();


        for(auto& module : moduleManager.GetModules()) {
            module.second->Unload();
        }
        registyCollection.registries.clear();

        for(auto& module : moduleManager.GetModules()) {
            module.second->Load();
        }
    });

}
