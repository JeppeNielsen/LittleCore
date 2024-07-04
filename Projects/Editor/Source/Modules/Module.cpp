//
// Created by Jeppe Nielsen on 13/01/2024.
//

#include "Module.hpp"
#include "../ScriptsInclude/ModuleFactory.hpp"
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

    createModuleFunction = reinterpret_cast<CreateModule>(dlsym(loadedLib, "CreateModule"));
    deleteModuleFunction = reinterpret_cast<DeleteModule>(dlsym(loadedLib, "DeleteModule"));

    ModuleFactory factory(engineContext);
    module = createModuleFunction(&factory);
}

void Module::Unload() {
    if (!IsLoaded()) {
        return;
    }
    deleteModuleFunction(module);
    dlclose(loadedLib);
    loadedLib = nullptr;
    module = nullptr;
}

bool Module::IsLoaded() {
    return loadedLib != nullptr;
}

void Module::Update(float dt) {
    if (!module) {
        return;
    }
    module->Update(dt);
}

void Module::Render() {
    if (!module) {
        return;
    }
    module->Render();
}

void Module::OnGui(ImGuiContext *imGuiContext) {
    if (!module) {
        return;
    }
    module->InvokeOnGui(imGuiContext);
}
