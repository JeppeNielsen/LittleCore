#include <entt/entt.hpp>
#include <iostream>

#include <chrono>
#include <ctime>
#include <cmath>
#include <dlfcn.h>

#include "Modules/Module.hpp"
#include "Modules/ModuleCompiler.hpp"
#include "Modules/ModuleDefinition.hpp"
#include "Modules/ModuleSettings.hpp"

#include <SDL3/SDL.h>
#include <thread>

int main_old() {

    SDL_Init(0);

    EngineContextFactory engineContextFactory(nullptr);
    EngineContext engineContext = engineContextFactory.Create();

    ModuleSettings moduleSettings;
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/");

    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/Files/Vector2.cpp");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/Files");


    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui_demo.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui_draw.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui_tables.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui_widgets.cpp");

    moduleSettings.libraryFolder = "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Cache/";

    ModuleDefinition consoleDefinition(moduleSettings, "Console",
                                             "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/ConsoleWindow/Console.cpp");

    ModuleDefinition hierarchyDefinition(moduleSettings, "Hierarchy",
                                                 "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/ConsoleWindow/Hierachy.cpp");

    auto resultConsole = consoleDefinition.Build();
    auto resultHierarchy = hierarchyDefinition.Build();

    Module console(engineContext, consoleDefinition);
    Module hierarchy(engineContext, hierarchyDefinition);

    console.Load();
    hierarchy.Load();

    bool exit = false;

    while (!exit) {

        SDL_Event event;

        while (SDL_PollEvent(&event)) {

            switch (event.type) {
                case SDL_EVENT_KEY_DOWN:
                    std::cout << event.key.keysym.scancode << std::endl;
                    break;
                case SDL_EVENT_QUIT:
                    exit = true;
                    break;

                case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
                    auto windowId = event.window.windowID;
                    SDL_DestroyWindow(SDL_GetWindowFromID(windowId));
                }
            }
        }

        console.Update(0.01f);
        console.Render();
        hierarchy.Update(0.1f);
        hierarchy.Render();


        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1000000ns);
    }

    console.Unload();
    hierarchy.Unload();

    return 0;
}


/*
struct Position {
    float x;
    float y;
};

struct Velocity {
    float dx;
    float dy;
};



int main() {

    entt::registry registry;
    entt::entity lastEntity;
    for (int i=0; i<100; i++) {
        auto entity = registry.create();
        registry.emplace<Position>(entity, Position{0,0});
        registry.emplace<Velocity>(entity, Velocity{i*0.1f,1-i*0.01f});
        lastEntity = entity;
    }

    auto view = registry.view<Position, const Velocity>();

    view.each([&registry] (entt::entity entity, Position& pos, const Velocity& velocity) {
        pos.x += velocity.dx;
        pos.y += velocity.dy;
    });

    std::cout << registry.get<Position>(lastEntity).x << "\n";
    std::cout << registry.get<Position>(lastEntity).y << "\n";


}
*/