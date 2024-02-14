//
// Created by Jeppe Nielsen on 12/02/2024.
//

#pragma once
#include "ShaderPlatform.hpp"
#include <string>

namespace LittleCore {
    struct ShaderCompilerSettings {
        ShaderPlatform platform;
        std::string shaderInputPath;
        std::string vertexWorkingPath;
        std::string fragmentWorkingPath;
        std::string varyingsWorkingPath;

        std::string vertexOutputPath;
        std::string fragmentOutputPath;

        std::string includePath;
    };

}