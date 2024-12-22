//
// Created by Jeppe Nielsen on 13/01/2024.
//

#pragma once
#include "ModuleDefinition.hpp"
#include "../ScriptsInclude/EngineContextFactory.hpp"
#include "../ScriptsInclude/IModule.hpp"

struct ModuleFactory;
struct EditorRenderer;

class Module {
public:
    Module(EngineContext& engineContext, ModuleDefinition& definition);

    void Load();
    void Unload();
    bool IsLoaded();

    void Update(float dt);
    void Render(EditorRenderer* editorRenderer);
    void OnGui(ImGuiContext* imGuiContext);

private:
    void* loadedLib = nullptr;
    ModuleDefinition& definition;
    EngineContext& engineContext;
    typedef IModule* (*CreateModule)(ModuleFactory*);
    typedef void (*DeleteModule)(IModule*);

    CreateModule createModuleFunction;
    DeleteModule deleteModuleFunction;
    IModule* module = nullptr;
};
