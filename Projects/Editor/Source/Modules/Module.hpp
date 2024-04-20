//
// Created by Jeppe Nielsen on 13/01/2024.
//

#pragma once
#include "ModuleDefinition.hpp"
#include "../ScriptsInclude/EngineContextFactory.hpp"
#include "../ScriptsInclude/IModuleState.hpp"

struct ModuleStateFactory;

class Module {
public:
    Module(EngineContext& engineContext, ModuleDefinition& definition);

    void Load();
    void Unload();
    bool IsLoaded();

    void Update(float dt);
    void Render();
    void OnGui(ImGuiContext* imGuiContext);

private:
    void* loadedLib = nullptr;
    ModuleDefinition& definition;
    EngineContext& engineContext;
    typedef IModuleState* (*CreateModuleState)(ModuleStateFactory*);
    typedef void (*DeleteModuleState)(IModuleState*);

    CreateModuleState createModuleStateFunction;
    DeleteModuleState deleteModuleStateFunction;
    IModuleState* state = nullptr;
};
