//
// Created by Jeppe Nielsen on 30/11/2024.
//

#include "Project.hpp"



Project::Project(EngineContext &engineContext) :
        moduleDefinitionsManager(moduleSettings),
        moduleManager(engineContext)
{

}


void Project::Reset() {
    moduleSettings = {};

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
}


void Project::LoadProject(const std::string rootPath) {

    Reset();

    projectSettings.rootPath = rootPath;// "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/TestProject";

    projectSettings.ModifyModules(moduleSettings.context, moduleDefinitionsManager);

    for(auto& definition : moduleDefinitionsManager.Definitions()) {
        moduleManager.AddModule(definition.first, *definition.second.get());
    }

    for(auto& module : moduleManager.GetModules()) {
        module.second->Load();
    }
}
