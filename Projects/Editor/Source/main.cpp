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

int main() {

    EngineContextFactory engineContextFactory;
    EngineContext engineContext = engineContextFactory.Create();

    ModuleSettings moduleSettings;
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/");

    moduleSettings.libraryFolder = "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Cache/";

    ModuleDefinition consoleDefinition(moduleSettings, "Console",
                                             "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/Scripts/ConsoleWindow/Console.cpp");

    ModuleDefinition hierachyDefinition(moduleSettings, "Hierarchy",
                                                 "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/Scripts/ConsoleWindow/Hierachy.cpp");

    auto resultConsole = consoleDefinition.Build();
    auto resultHierachy = hierachyDefinition.Build();

    Module console(engineContext, consoleDefinition);
    Module hierachy(engineContext, hierachyDefinition);

    console.Load();
    hierachy.Load();

    for (int i=0; i<10; i++) {
        console.Update(0.01f);
        console.Render();
        hierachy.Update(0.1f);
        hierachy.Render();
    }

    console.Unload();
    hierachy.Unload();

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