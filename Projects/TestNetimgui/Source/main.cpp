//
// Created by Jeppe Nielsen on 13/04/2024.
//
#include "Engine.hpp"
#include "ImGuiController.hpp"
#include "NetimguiServerController.hpp"
#include <iostream>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include "Project.hpp"

using namespace LittleCore;

struct ImguiTest : IState {
    ImGuiController gui;
    NetimguiServerController netimguiController;
    Project project;

    ImguiTest() : netimguiController(gui) {}

    void Initialize() override {
        gui.Initialize(mainWindow, [this]() {
            OnGUI();
        });


        if (!netimguiController.Start()) {
            std::cout << "Netimgui lib failed\n";
        }
        project.projectSettings.engineRoot = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimgui/Assets/";

        gui.LoadFont(project.projectSettings.engineRoot + "Fonts/LucidaG.ttf", 12);

        project.LoadProject("/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/Assets");
        project.moduleDefinitionsManager.CreateFromMainFile("SimpleGame", "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/Assets/Code/SimpleGame.cpp");
    }

    void HandleEvent(void* event) override {
        gui.HandleEvent(event);
    }

    void StartProgram() {

        pid_t pid = fork();
        if (pid == -1) {
            std::perror("fork");
            return;
        }

        if (pid == 0) { // Child
            auto& definition = project.moduleDefinitionsManager.Definitions().at("SimpleGame");
            std::string exePath = definition->LibraryPath();
            system(exePath.c_str());
        }
    }

    void StartProgram2() {
        /*pid_t pid = fork();
        if (pid == -1) {
            std::perror("fork");
            return;
        }
         */

        //if (pid == 0) { // Child


            char *path = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Build/bin/ARM64/Debug/TestNetimguiClient";

            pid_t pid;
            char *argv[] = {
                    const_cast<char*>("open"),
                    const_cast<char*>("-g"), // do not activate
                    const_cast<char*>("-j"), // launch hidden
                    const_cast<char*>("-a"),
                    const_cast<char*>(path),
                    nullptr
            };

            extern char **environ;
            int rc = posix_spawn(&pid, "/usr/bin/open", nullptr, nullptr, argv, environ);
        //}


    }

    void Compile() {
        auto& definition = project.moduleDefinitionsManager.Definitions().at("SimpleGame");

        definition->StartBuild();

        /*for(auto error : result.errors) {
            std::cout<< error<<"\n";
        }*/


    }

    void OnGUI() {
        ImGui::DockSpaceOverViewport();
        ImGui::Begin("Editor");

        if (ImGui::Button("Compile")) {
            Compile();
        }

        if (ImGui::Button("Start process")) {
            StartProgram();
        }

        ImGui::End();
        netimguiController.Draw();
    }

    void Update(float dt) override {
        for(auto& m : project.moduleDefinitionsManager.Definitions()) {
            m.second->Update();
        }
    }

    void Render() override {
        netimguiController.Update();
       gui.Render();
    }
};

int main() {
    Engine e({.mainWindowTitle = "Test Netimgui"});
    e.Start<ImguiTest>();
    return 0;
}

