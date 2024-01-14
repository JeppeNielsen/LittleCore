//
// Created by Jeppe Nielsen on 13/01/2024.
//

#pragma once
#include "ModuleDefinition.hpp"
#include "../ScriptsInclude/IModuleState.hpp"

struct ModuleStateFactory;

class Module {
public:
    Module(ModuleDefinition& definition);

    void Load();
    void Unload();
    bool IsLoaded();

    void Update(float dt);
    void Render();

private:
    void* loadedLib = nullptr;
    ModuleDefinition& definition;
    typedef IModuleState* (*CreateModuleState)(ModuleStateFactory*);
    typedef void (*DeleteModuleState)(IModuleState*);

    CreateModuleState createModuleStateFunction;
    DeleteModuleState deleteModuleStateFunction;
    IModuleState* state = nullptr;
};
