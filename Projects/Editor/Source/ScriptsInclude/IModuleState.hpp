//
// Created by Jeppe Nielsen on 13/01/2024.
//

#pragma once
#include "EngineContext.hpp"

struct IModuleState {
    virtual ~IModuleState() {}
    virtual void Initialize(EngineContext& engineContext) = 0;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
};