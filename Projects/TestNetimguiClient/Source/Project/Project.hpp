//
// Created by Jeppe Nielsen on 13/10/2025.
//
#pragma once
#include <string>
#include "ResourcePathMapper.hpp"
#include "ResourceHashMapper.hpp"

namespace LittleCore {
    struct Project {

        std::string rootPath;
        ResourcePathMapper resourcePathMapper;
        ResourceHashMapper resourceHashMapper;

    };
}
