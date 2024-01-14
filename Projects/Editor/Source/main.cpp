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

    ModuleSettings moduleSettings;
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/");

    moduleSettings.libraryFolder = "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Cache/";

    ModuleDefinition consoleWindowDefinition(moduleSettings, "ConsoleWindow",
                                             "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/Scripts/ConsoleWindow/ConsoleWindow.cpp");



    if (consoleWindowDefinition.LibraryExists()) {
        std::cout << "Library exists \n";
    } else {
        std::cout << "Library does not exists \n";
    }

    auto result = consoleWindowDefinition.Build();

    if (result.errors.empty()) {
        std::cout << "Compilation succesful\n";
    } else {
        for (auto error: result.errors) {
            std::cout << error << std::endl;
        }
    }

    if (consoleWindowDefinition.LibraryExists()) {
        std::cout << "Library exists \n";
    } else {
        std::cout << "Library does not exists \n";
    }

    Module consoleWindow(consoleWindowDefinition);

    consoleWindow.Load();

    for (int i=0; i<1000; i++) {
        consoleWindow.Update(0.01f);
        consoleWindow.Render();
    }

    consoleWindow.Unload();

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