//
// Created by Jeppe Nielsen on 12/02/2024.
//

#pragma once
#include "ShaderParserResult.hpp"

namespace LittleCore {
    class ShaderParser {
    public:

        ShaderParserResult TryParse(const std::string& shaderSource);

    };
}
