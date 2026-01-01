//
// Created by Jeppe Nielsen on 13/01/2024.
//

#include "Module.hpp"
#include <dlfcn.h>
#include <iostream>

Module::Module(ModuleDefinition &definition) : definition(definition) {
}

void Module::Load() {
    if (IsLoaded()) {
        return;
    }

    if (!definition.LibraryExists()) {
        return;
    }
    
    std::string path = definition.LibraryPath();
}

void Module::Unload() {
    if (!IsLoaded()) {
        return;
    }
}

bool Module::IsLoaded() {
    return false;
}

void Module::Update(float dt) {

}

void Module::Render(EditorRenderer* editorRenderer) {

}

void Module::OnGui() {

}
