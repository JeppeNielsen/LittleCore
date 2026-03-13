#pragma once

#include "SokolDirect.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace LittleCore {
    struct CompiledShaderUniformInfo {
        std::string name;
        std::string glslName;
        sg_uniform_type type = SG_UNIFORMTYPE_INVALID;
        uint16_t arrayCount = 1;
    };

    struct CompiledShaderUniformBlockInfo {
        uint8_t slot = 0;
        sg_shader_stage stage = SG_SHADERSTAGE_NONE;
        sg_uniform_layout layout = SG_UNIFORMLAYOUT_STD140;
        uint32_t size = 0;
        int hlslRegisterBN = -1;
        int mslBufferN = -1;
        int wgslGroup0BindingN = -1;
        std::vector<CompiledShaderUniformInfo> uniforms;
        std::vector<CompiledShaderUniformInfo> glslUniforms;
    };

    struct CompiledShaderStageInfo {
        std::string source;
        std::string entryPoint;
    };

    struct CompiledShaderInfo {
        CompiledShaderStageInfo vertexShader;
        CompiledShaderStageInfo fragmentShader;
        std::array<std::string, 3> attributeNames = {"a_position", "a_color0", "a_texcoord0"};
        std::vector<CompiledShaderUniformBlockInfo> uniformBlocks;
    };
}
