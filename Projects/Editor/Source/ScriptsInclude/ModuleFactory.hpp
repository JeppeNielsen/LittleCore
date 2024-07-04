//
// Created by Jeppe Nielsen on 14/01/2024.
//

#pragma once
#include "EngineContext.hpp"
#include "IModule.hpp"

class ModuleFactory {
public:
    ModuleFactory(EngineContext& engineContext) : engineContext(engineContext) {  }

    template<typename T>
    T* CreateModule() {
        T* state = new T();
        state->Initialize(engineContext);
        return state;
    }
private:
    EngineContext& engineContext;

};

#define CreateModule(className)             \
extern "C" {                                            \
    IModule* CreateModule(ModuleFactory* factory) {\
        return factory->CreateModule<className>();      \
    }                                                   \
                                                        \
    void DeleteModule(IModule* state) {       \
        delete state;                                   \
    }                                                   \
}

