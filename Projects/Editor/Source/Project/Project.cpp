//
// Created by Jeppe Nielsen on 30/11/2024.
//

#include "Project.hpp"



Project::Project(EngineContext &engineContext) :
        moduleDefinitionsManager(moduleSettings),
        moduleManager(engineContext),
        defaultResourceManager(resourcePathMapper)
{
    defaultResourceManager.CreateLoaderFactory<LittleCore::ShaderResourceLoaderFactory>();
    defaultResourceManager.CreateLoaderFactory<LittleCore::TextureResourceLoaderFactory>();
}


void Project::Reset() {
    moduleSettings = {};

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ComponentEditors/");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ComponentEditors/GuiHelper.cpp");
    
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/EditorRenderer.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/ResourceLoader.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/RegistryManager.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/ComponentDrawer.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/ComponentFactory.cpp");



    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/");

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/misc/cpp/");

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/entt/single_include/");

    //glm
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/glm/");

    //bgfx
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/bgfx/include/");

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/bx/include/");

    moduleSettings.context.defines.push_back("BX_CONFIG_DEBUG=0");

    //stb
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/stb/");

    //files
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Files/");

    //math
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Math/");

    //Logic headers
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Hierarchy/");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Input/");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Movement/");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Rendering/");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Simulation/");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Spatial/");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Transform/");

    //Logic classes
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/ResourceManagement/");

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Tuples/");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Utilities/");

    //libs
    moduleSettings.context.libPaths.push_back("/Users/jeppe/Jeppes/LittleCore/bin/Debug");
    moduleSettings.context.libs.push_back("LittleCore");
    //moduleSettings.context.libs.push_back("bgfx");
    moduleSettings.context.libs.push_back("bx");
    //moduleSettings.context.libs.push_back("bimg");
    moduleSettings.context.libs.push_back("ImGui");


    /*
     *
     * "SDL",
      "bgfx",
      "bimg",
      "bx",
      "LittleCore",
      "ShaderCompiler",
      "ImGui"
     *
     *
     */

    moduleSettings.libraryFolder = "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Cache/";
}


void Project::LoadProject(const std::string rootPath) {

    Reset();

    resourcePathMapper.RefreshFromRootPath(rootPath);

    projectSettings.rootPath = rootPath;// "/Users/jeppe/Jeppes/LittleCore/Projects/Editor/TestProject";

    projectSettings.ModifyModules(moduleSettings.context, moduleDefinitionsManager);

    for(auto& definition : moduleDefinitionsManager.Definitions()) {
        moduleManager.AddModule(definition.first, *definition.second.get());
    }

    for(auto& module : moduleManager.GetModules()) {
        module.second->Load();
    }
}
