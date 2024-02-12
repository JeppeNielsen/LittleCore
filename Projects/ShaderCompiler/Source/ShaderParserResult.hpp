//
// Created by Jeppe Nielsen on 12/02/2024.
//

#pragma once
#include <string>

namespace LittleCore {

    struct ShaderParserResult {
        bool succes;
        std::string varyings;
        std::string vertex;
        std::string fragment;
    };

}