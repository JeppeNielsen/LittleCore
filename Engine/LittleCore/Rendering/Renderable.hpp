//
// Created by Jeppe Nielsen on 21/01/2024.
//
#pragma once
#include <bgfx/bgfx.h>

namespace LittleCore {

    struct Renderable {

        bgfx::ProgramHandle shaderProgram;
        int mask;

    };

}