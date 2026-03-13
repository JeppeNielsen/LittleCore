//
// Created by Jeppe Nielsen on 14/03/2024.
//

#pragma once
#include "SokolDirect.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace LittleCore {
    struct ShaderUniformInfo {
        std::string name;
        sg_uniform_type type = SG_UNIFORMTYPE_INVALID;
        uint16_t arrayCount = 1;
    };

    struct ShaderUniformBlockInfo {
        uint8_t slot = 0;
        sg_shader_stage stage = SG_SHADERSTAGE_NONE;
        sg_uniform_layout layout = SG_UNIFORMLAYOUT_STD140;
        uint32_t size = 0;
        std::vector<ShaderUniformInfo> uniforms;
    };

    struct ShaderResource {
        sg_shader handle = {SG_INVALID_ID};
        std::vector<ShaderUniformBlockInfo> uniformBlocks;
    };
}
