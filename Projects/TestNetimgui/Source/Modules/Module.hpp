//
// Created by Jeppe Nielsen on 13/01/2024.
//

#pragma once
#include "ModuleDefinition.hpp"

struct ModuleFactory;
struct EditorRenderer;

class Module {
public:
    Module(ModuleDefinition& definition);

    void Load();
    void Unload();
    bool IsLoaded();

    void Update(float dt);
    void Render(EditorRenderer* editorRenderer);
    void OnGui();

private:
    ModuleDefinition& definition;
};
