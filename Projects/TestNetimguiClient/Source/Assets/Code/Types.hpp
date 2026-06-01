//
// Created by Jeppe Nielsen on 29/03/2026.
//

#pragma once
#include "MetaHelper.hpp"
#include "glm/glm.hpp"
#include "Rect.hpp"
#include "Color.hpp"

struct Velocity {
    glm::vec3 speed;
};

struct Rotater {
    float speed;
};

struct Bobber {
    float speed = 1;
    float amplitude = 1;
    float progress = 0;
};

struct Bat {
    LittleCore::Rect bounds;
};

struct Ball {
    float radius;
};

struct ClickColorer {
    LittleCore::Color downColor;
    LittleCore::Color upColor;
};

using Types = LittleCore::Meta::TypeList<Velocity, Rotater, Bobber, Bat, Ball, ClickColorer>;
