//
// Created by Jeppe Nielsen on 29/03/2026.
//

#pragma once
#include "LocalTransform.hpp"
#include "SystemBase.hpp"
#include "Types.hpp"

using namespace LittleCore;

struct MoverSystem : LittleCore::SystemBase {
    void Update(float dt);
};

struct RotatorSystem : LittleCore::SystemBase {
    void Update(float dt);
};

struct BobberSystem : LittleCore::SystemBase {
    void Update(float dt);
};

struct CollisionSystem : LittleCore::SystemBase {
    void Update(float dt);
};
