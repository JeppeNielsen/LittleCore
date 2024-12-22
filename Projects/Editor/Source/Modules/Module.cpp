//
// Created by Jeppe Nielsen on 13/01/2024.
//

#include "Module.hpp"
#include "../ScriptsInclude/ModuleFactory.hpp"
#include <dlfcn.h>
#include <iostream>

Module::Module(EngineContext& engineContext, ModuleDefinition &definition) : engineContext(engineContext), definition(definition) {
}

void Module::Load() {
    if (IsLoaded()) {
        return;
    }

    if (!definition.LibraryExists()) {
        return;
    }
    
    std::string path = definition.LibraryPath();

    loadedLib = dlopen(path.c_str(), RTLD_LAZY);

    if (loadedLib == nullptr) {
        std::string error = dlerror();
        std::cout << "error from loaded lib: "<<error<<"\n";
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

void Module::Render(EditorRenderer* editorRenderer) {
    if (!module) {
        return;
    }
    module->Render(editorRenderer);
}

void Module::OnGui(ImGuiContext *imGuiContext) {
    if (!module) {
        return;
    }
    module->InvokeOnGui(imGuiContext);
}
