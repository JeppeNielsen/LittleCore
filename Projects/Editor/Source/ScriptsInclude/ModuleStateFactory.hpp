//
// Created by Jeppe Nielsen on 14/01/2024.
//

#pragma once
#include "EngineContext.hpp"
#include "IModuleState.hpp"

class ModuleStateFactory {
public:
    ModuleStateFactory(EngineContext& engineContext) : engineContext(engineContext) {  }

    template<typename T>
    T* CreateState() {
        T* state = new T();
        state->Initialize(engineContext);
        return state;
    }
private:
    EngineContext& engineContext;

};
