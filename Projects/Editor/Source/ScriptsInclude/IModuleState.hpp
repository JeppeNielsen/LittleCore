//
// Created by Jeppe Nielsen on 13/01/2024.
//

#pragma once

struct IModuleState {
    virtual ~IModuleState() {}
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
};