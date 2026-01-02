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

        gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/TestImGui/Source/Fonts/LucidaG.ttf", 12);
        if (!netimguiController.Start()) {
            std::cout << "Netimgui lib failed\n";
        }

        project.LoadProject("/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/Assets");
        project.moduleDefinitionsManager.CreateFromMainFile("Main", "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/main.cpp");


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


            const char *path = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Build/bin/ARM64/Debug/TestNetimguiClient";
            system(path);


/*
            char *argv[] = { (char*)path,
                             const_cast<char*>("-g"), // do not activate
                             const_cast<char*>("-j"), // launch hidden
                             const_cast<char*>("-a"),
                             nullptr };

            execvp(path, argv);
            // If execvp returns, it failed:
            std::perror("execvp");
            _exit(127);
            */
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
        auto& definition = project.moduleDefinitionsManager.Definitions().at("Main");

        auto result = definition->Build();

        for(auto error : result.errors) {
            std::cout<< error<<"\n";
        }

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

