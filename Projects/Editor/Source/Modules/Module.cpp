//
// Created by Jeppe Nielsen on 13/01/2024.
//

#include "Module.hpp"
#include "../ScriptsInclude/ModuleStateFactory.hpp"
#include <dlfcn.h>

Module::Module(EngineContext& engineContext, ModuleDefinition &definition) : engineContext(engineContext), definition(definition) {
}

void Module::Load() {
    if (IsLoaded()) {
        return;
    }

    if (!definition.LibraryExists()) {
        return;
    }

    loadedLib = dlopen(definition.LibraryPath().c_str(), RTLD_LAZY);

    if (loadedLib == nullptr) {
        return;
    }

    createModuleStateFunction = reinterpret_cast<CreateModuleState>(dlsym(loadedLib, "CreateModuleState"));
    deleteModuleStateFunction = reinterpret_cast<DeleteModuleState>(dlsym(loadedLib, "DeleteModuleState"));

    ModuleStateFactory factory(engineContext);
    state = createModuleStateFunction(&factory);
}

void Module::Unload() {
    if (!IsLoaded()) {
        return;
    }
    deleteModuleStateFunction(state);
    dlclose(loadedLib);
    loadedLib = nullptr;
    state = nullptr;
}

bool Module::IsLoaded() {
    return loadedLib != nullptr;
}

void Module::Update(float dt) {
    if (!state) {
        return;
    }
    state->Update(dt);
}

void Module::Render() {
    if (!state) {
        return;
    }
    state->Render();
}

void Module::OnGui(ImGuiContext *imGuiContext) {
    if (!state) {
        return;
    }
    state->InvokeOnGui(imGuiContext);
}
