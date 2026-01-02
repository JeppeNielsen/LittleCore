//
// Created by Jeppe Nielsen on 30/11/2024.
//

#include "Project.hpp"



Project::Project() :
        moduleDefinitionsManager(moduleSettings)
{

}


void Project::Reset() {
    moduleSettings = {};

    std::string engineRoot = "/Users/jeppe/Jeppes/LittleCore/";


    moduleSettings.context.hppFiles.push_back(engineRoot + "External/imgui/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "External/imgui/misc/cpp/");


    moduleSettings.context.hppFiles.push_back(engineRoot + "External/entt/src/");

    //glm
    moduleSettings.context.hppFiles.push_back(engineRoot + "External/glm/");

    //bgfx
    moduleSettings.context.hppFiles.push_back(engineRoot + "External/bgfx/include/");

    moduleSettings.context.hppFiles.push_back(engineRoot + "External/bx/include/");

    //stb
    moduleSettings.context.hppFiles.push_back(engineRoot + "External/stb/");

    //glaze
    moduleSettings.context.hppFiles.push_back(engineRoot + "External/glaze/include");

    //
    moduleSettings.context.hppFiles.push_back(engineRoot + "External/ImGuizmo/");

    //Application

    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/Application/EditorSimulations/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/Application/Gui/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/Application/Project/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/Application/State/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/Application/Utilities/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/Application/Windows/");



    //Core
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Core/");

    //Fibers
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Fibers/");

    //files
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Files/");

    //Logic headers
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Logic/Coloring/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Logic/Fonts/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Logic/Hierarchy/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Logic/Input/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Logic/Labels/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Logic/Movement/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Logic/Picking/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Logic/Prefabs/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Logic/Registry/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Logic/Rendering/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Logic/Simulation/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Logic/Spatial/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Logic/Systems/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Logic/Transform/");


    //math
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Math/");


    //meta
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Meta/");

    //Reflection
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Reflection/");

    //Resources
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/ResourceLoaders/");
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/ResourceManagement/");

    //Serialization
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Serialization/");

    //Tuples
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Tuples/");

    //Utilities
    moduleSettings.context.hppFiles.push_back(engineRoot + "Engine/LittleCore/Utilities/");


    //libs
    moduleSettings.context.libPaths.push_back(engineRoot + "bin/Debug");





    /*
     links {
      "SDL",
      "bgfx",
      "bimg",
      "bx",
      "LittleCore",
      "ImGui",
      "Netimgui"
   }
     */


    moduleSettings.context.libs.push_back("SDL");
    moduleSettings.context.libs.push_back("bgfx");
    moduleSettings.context.libs.push_back("bimg");
    moduleSettings.context.libs.push_back("bx");
    moduleSettings.context.libs.push_back("LittleCore");
    moduleSettings.context.libs.push_back("ImGui");
    moduleSettings.context.libs.push_back("Netimgui");
    moduleSettings.context.libs.push_back("Application");
    moduleSettings.context.libs.push_back("ShaderCompiler");
    moduleSettings.context.libs.push_back("shaderc");
    moduleSettings.context.libs.push_back("spirv-cross");
    moduleSettings.context.libs.push_back("spirv-opt");
    moduleSettings.context.libs.push_back("glslang");
    moduleSettings.context.libs.push_back("glsl-optimizer");
    moduleSettings.context.libs.push_back("fcpp");


    /*
     links {

      }
     */
    moduleSettings.context.frameworks.push_back("QuartzCore");
    moduleSettings.context.frameworks.push_back("Cocoa");
    moduleSettings.context.frameworks.push_back("IOKit");
    moduleSettings.context.frameworks.push_back("Carbon");
    moduleSettings.context.frameworks.push_back("CoreAudio");
    moduleSettings.context.frameworks.push_back("AudioToolbox");
    moduleSettings.context.frameworks.push_back("Metal");
    moduleSettings.context.frameworks.push_back("OpenGL");

    moduleSettings.context.defines.push_back("BX_CONFIG_DEBUG=0");
    moduleSettings.context.defines.push_back("XWIN_COCOA=1");
    moduleSettings.context.defines.push_back("NDEBUG");

    moduleSettings.libraryFolder = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimgui/Cache/";
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
