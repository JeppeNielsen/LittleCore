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
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ComponentEditors/Editors/LocalTransformEditor.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ComponentEditors/Editors/LocalBoundingBoxEditor.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ComponentEditors/Editors/CameraEditor.cpp");


    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/EditorRenderer.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/ResourceLoader.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Source/ScriptsInclude/RegistryManager.cpp");


    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui_demo.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui_draw.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui_tables.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/imgui_widgets.cpp");

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/misc/cpp/");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/misc/cpp/imgui_stdlib.cpp");

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/imgui/misc/cpp/");

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/entt/single_include/");

    //glm
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/glm/");

    //bgfx
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/bgfx/include/");
    //moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/bgfx/src/bgfx.cpp");

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/bx/include/");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/bx/src/math.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/bx/src/bx.cpp");

    moduleSettings.context.defines.push_back("BX_CONFIG_DEBUG=0");

    //stb

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/External/stb/");

    //files
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Files/");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Files/FileHelper.cpp");

    //math
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Math/");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Math/BoundingBox.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Math/BoundingFrustum.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Math/MeshIntersector.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Math/Plane.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Math/Ray.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Math/Rect.cpp");



    //Logic headers
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Hierarchy/");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Input/");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Movement/");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Rendering/");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Simulation/");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Spatial/");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Transform/");

    //Logic classes
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Hierarchy/HierarchySystem.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Input/InputKeyMapper.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Input/InputSystem.cpp");

    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Movement/MovableSystem.cpp");

    //moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Rendering/BgfxRenderer.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Rendering/Camera.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Rendering/RenderSystem.cpp");

    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Spatial/MeshBoundingBoxSystem.cpp");
    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Spatial/WorldBoundingBoxSystem.cpp");

    moduleSettings.context.cppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Logic/Transform/WorldTransformSystem.cpp");

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/ResourceManagement/");

    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Tuples/");
    moduleSettings.context.hppFiles.push_back("/Users/jeppe/Jeppes/LittleCore/Engine/LittleCore/Utilities/");

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
