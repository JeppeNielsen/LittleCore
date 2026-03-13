//
// Created by Jeppe Nielsen on 12/02/2024.
//

#include "ShaderCompiler.hpp"
#include "CompiledShaderInfo.hpp"

#include <array>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

#include "args.h"
#include "bytecode.h"
#include "generators/generate.h"
#include "input.h"
#include "reflection.h"
#include "spirv.h"
#include "spirvcross.h"
#include "FileHelper.hpp"

using namespace LittleCore;

namespace {
    struct SpirvToolProcessScope {
        SpirvToolProcessScope() {
            shdc::Spirv::initialize_spirv_tools();
        }
        ~SpirvToolProcessScope() {
            shdc::Spirv::finalize_spirv_tools();
        }
    };

    void SetError(std::string* errorMessage, const std::string& message) {
        if (errorMessage != nullptr) {
            *errorMessage = message;
        }
    }

    bool TryParseSlang(const std::string& slangName, shdc::Slang::Enum& outSlang) {
        for (int i = 0; i < shdc::Slang::Num; ++i) {
            const auto slang = shdc::Slang::from_index(i);
            if (slangName == shdc::Slang::to_str(slang)) {
                outSlang = slang;
                return true;
            }
        }
        return false;
    }

    bool HasErrorMessages(const std::vector<shdc::ErrMsg>& messages, shdc::ErrMsg::Format format, std::string* errorMessage) {
        for (const auto& message : messages) {
            if (message.type == shdc::ErrMsg::ERROR) {
                SetError(errorMessage, message.as_string(format));
                return true;
            }
        }
        return false;
    }

    uint32_t RoundUp(const uint32_t value, const uint32_t alignment) {
        if (alignment == 0) {
            return value;
        }
        const uint32_t remainder = value % alignment;
        return remainder == 0 ? value : (value + alignment - remainder);
    }

    sg_shader_stage ToSgShaderStage(const shdc::ShaderStage::Enum stage) {
        switch (stage) {
            case shdc::ShaderStage::Vertex:
                return SG_SHADERSTAGE_VERTEX;
            case shdc::ShaderStage::Fragment:
                return SG_SHADERSTAGE_FRAGMENT;
            default:
                return SG_SHADERSTAGE_NONE;
        }
    }

    sg_uniform_type ToSgUniformType(const shdc::refl::Type::Enum type) {
        switch (type) {
            case shdc::refl::Type::Float:
                return SG_UNIFORMTYPE_FLOAT;
            case shdc::refl::Type::Float2:
                return SG_UNIFORMTYPE_FLOAT2;
            case shdc::refl::Type::Float3:
                return SG_UNIFORMTYPE_FLOAT3;
            case shdc::refl::Type::Float4:
                return SG_UNIFORMTYPE_FLOAT4;
            case shdc::refl::Type::Int:
                return SG_UNIFORMTYPE_INT;
            case shdc::refl::Type::Int2:
                return SG_UNIFORMTYPE_INT2;
            case shdc::refl::Type::Int3:
                return SG_UNIFORMTYPE_INT3;
            case shdc::refl::Type::Int4:
                return SG_UNIFORMTYPE_INT4;
            case shdc::refl::Type::Mat4x4:
                return SG_UNIFORMTYPE_MAT4;
            default:
                return SG_UNIFORMTYPE_INVALID;
        }
    }

    sg_uniform_type ToFlattenedSgUniformType(const shdc::refl::Type::Enum type) {
        switch (type) {
            case shdc::refl::Type::Float:
            case shdc::refl::Type::Float2:
            case shdc::refl::Type::Float3:
            case shdc::refl::Type::Float4:
            case shdc::refl::Type::Mat4x4:
                return SG_UNIFORMTYPE_FLOAT4;
            case shdc::refl::Type::Int:
            case shdc::refl::Type::Int2:
            case shdc::refl::Type::Int3:
            case shdc::refl::Type::Int4:
                return SG_UNIFORMTYPE_INT4;
            default:
                return SG_UNIFORMTYPE_INVALID;
        }
    }

    uint16_t ToArrayCount(const int arrayCount) {
        return static_cast<uint16_t>(std::max(1, arrayCount));
    }

    std::string GetSlangFileExtension(const shdc::Slang::Enum slang) {
        if (shdc::Slang::is_glsl(slang)) {
            return ".glsl";
        }
        if (shdc::Slang::is_hlsl(slang)) {
            return ".hlsl";
        }
        if (shdc::Slang::is_msl(slang)) {
            return ".metal";
        }
        if (shdc::Slang::is_wgsl(slang)) {
            return ".wgsl";
        }
        return "";
    }

    std::filesystem::path GetBareOutputPath(const std::string& outputBasePath,
                                            const std::string& moduleName,
                                            const std::string& programName,
                                            const shdc::ShaderStage::Enum stage,
                                            const shdc::Slang::Enum slang) {
        const std::string modulePrefix = moduleName.empty() ? "" : (moduleName + "_");
        return std::filesystem::path(
            outputBasePath + "_" +
            modulePrefix +
            programName + "_" +
            shdc::Slang::to_str(slang) + "_" +
            shdc::ShaderStage::to_str(stage) +
            GetSlangFileExtension(slang));
    }

    bool PopulateCompiledShaderInfo(const std::string& outputBasePath,
                                    const shdc::Input& input,
                                    const shdc::Slang::Enum outputSlang,
                                    const shdc::refl::Reflection& reflection,
                                    CompiledShaderInfo& compiledShaderInfo,
                                    std::string* errorMessage) {
        if (reflection.progs.empty()) {
            SetError(errorMessage, "shader reflection did not contain any programs");
            return false;
        }
        if (reflection.progs.size() != 1) {
            SetError(errorMessage, "metadata extraction only supports shader files with a single @program");
            return false;
        }

        const shdc::refl::ProgramReflection& program = reflection.progs.front();
        if (!program.has_vs() || !program.has_fs()) {
            SetError(errorMessage, "metadata extraction only supports vertex/fragment shaders");
            return false;
        }

        const std::filesystem::path vertexPath = GetBareOutputPath(outputBasePath, input.module, program.name, shdc::ShaderStage::Vertex, outputSlang);
        const std::filesystem::path fragmentPath = GetBareOutputPath(outputBasePath, input.module, program.name, shdc::ShaderStage::Fragment, outputSlang);

        compiledShaderInfo.vertexShader.source = FileHelper::ReadAllText(vertexPath.string());
        compiledShaderInfo.fragmentShader.source = FileHelper::ReadAllText(fragmentPath.string());
        if (compiledShaderInfo.vertexShader.source.empty() || compiledShaderInfo.fragmentShader.source.empty()) {
            SetError(errorMessage, "failed to read compiled shader stage output");
            return false;
        }

        compiledShaderInfo.vertexShader.entryPoint = program.vs().entry_point_by_slang(outputSlang);
        compiledShaderInfo.fragmentShader.entryPoint = program.fs().entry_point_by_slang(outputSlang);

        for (const auto& inputAttr : program.vs().inputs) {
            if (inputAttr.slot < 0 || inputAttr.slot >= static_cast<int>(compiledShaderInfo.attributeNames.size())) {
                continue;
            }
            if (!inputAttr.name.empty()) {
                compiledShaderInfo.attributeNames[inputAttr.slot] = inputAttr.name;
            }
        }

        compiledShaderInfo.uniformBlocks.clear();
        for (const auto& reflectedUniformBlock : program.bindings.uniform_blocks) {
            const sg_shader_stage stage = ToSgShaderStage(reflectedUniformBlock.stage);
            if (stage == SG_SHADERSTAGE_NONE) {
                SetError(errorMessage, "unsupported shader stage in reflected uniform block");
                return false;
            }

            CompiledShaderUniformBlockInfo uniformBlockInfo;
            uniformBlockInfo.slot = static_cast<uint8_t>(reflectedUniformBlock.sokol_slot);
            uniformBlockInfo.stage = stage;
            uniformBlockInfo.layout = SG_UNIFORMLAYOUT_STD140;
            uniformBlockInfo.size = RoundUp(static_cast<uint32_t>(reflectedUniformBlock.struct_info.size), 16);
            uniformBlockInfo.hlslRegisterBN = reflectedUniformBlock.hlsl_register_b_n;
            uniformBlockInfo.mslBufferN = reflectedUniformBlock.msl_buffer_n;
            uniformBlockInfo.wgslGroup0BindingN = reflectedUniformBlock.wgsl_group0_binding_n;

            for (const auto& structItem : reflectedUniformBlock.struct_info.struct_items) {
                const sg_uniform_type uniformType = ToSgUniformType(structItem.type);
                if (uniformType == SG_UNIFORMTYPE_INVALID) {
                    SetError(errorMessage, "unsupported reflected uniform type: " + structItem.type_as_glsl());
                    return false;
                }

                CompiledShaderUniformInfo uniformInfo;
                uniformInfo.name = structItem.name;
                uniformInfo.glslName = structItem.name;
                uniformInfo.type = uniformType;
                uniformInfo.arrayCount = ToArrayCount(structItem.array_count);
                uniformBlockInfo.uniforms.push_back(uniformInfo);
            }

            if (reflectedUniformBlock.flattened) {
                if (reflectedUniformBlock.struct_info.struct_items.empty()) {
                    SetError(errorMessage, "flattened uniform block had no reflected items");
                    return false;
                }

                const sg_uniform_type flattenedType = ToFlattenedSgUniformType(reflectedUniformBlock.struct_info.struct_items.front().type);
                if (flattenedType == SG_UNIFORMTYPE_INVALID) {
                    SetError(errorMessage, "unsupported flattened uniform block type");
                    return false;
                }

                CompiledShaderUniformInfo glslUniformInfo;
                glslUniformInfo.name = reflectedUniformBlock.name;
                glslUniformInfo.glslName = reflectedUniformBlock.name;
                glslUniformInfo.type = flattenedType;
                glslUniformInfo.arrayCount = static_cast<uint16_t>(uniformBlockInfo.size / 16);
                uniformBlockInfo.glslUniforms.push_back(glslUniformInfo);
            } else {
                for (const auto& structItem : reflectedUniformBlock.struct_info.struct_items) {
                    const sg_uniform_type uniformType = ToSgUniformType(structItem.type);
                    if (uniformType == SG_UNIFORMTYPE_INVALID) {
                        SetError(errorMessage, "unsupported reflected GLSL uniform type: " + structItem.type_as_glsl());
                        return false;
                    }

                    CompiledShaderUniformInfo glslUniformInfo;
                    glslUniformInfo.name = structItem.name;
                    glslUniformInfo.glslName = reflectedUniformBlock.inst_name.empty()
                        ? structItem.name
                        : (reflectedUniformBlock.inst_name + "." + structItem.name);
                    glslUniformInfo.type = uniformType;
                    glslUniformInfo.arrayCount = ToArrayCount(structItem.array_count);
                    uniformBlockInfo.glslUniforms.push_back(glslUniformInfo);
                }
            }

            compiledShaderInfo.uniformBlocks.push_back(std::move(uniformBlockInfo));
        }

        std::sort(compiledShaderInfo.uniformBlocks.begin(),
                  compiledShaderInfo.uniformBlocks.end(),
                  [](const CompiledShaderUniformBlockInfo& lhs, const CompiledShaderUniformBlockInfo& rhs) {
                      return lhs.slot < rhs.slot;
                  });

        return true;
    }
}

bool ShaderCompiler::CompileSokolBare(const std::string& inputPath,
                                      const std::string& outputPath,
                                      const std::string& slang,
                                      CompiledShaderInfo* compiledShaderInfo,
                                      std::string* errorMessage) {
    if (inputPath.empty() || outputPath.empty() || slang.empty()) {
        SetError(errorMessage, "invalid shader compiler input arguments");
        return false;
    }

    shdc::Slang::Enum outputSlang = shdc::Slang::REFLECTION;
    if (!TryParseSlang(slang, outputSlang) || outputSlang == shdc::Slang::REFLECTION) {
        SetError(errorMessage, "unsupported shader slang: " + slang);
        return false;
    }

    shdc::Args args;
    args.valid = true;
    args.exit_code = 0;
    args.input = inputPath;
    args.output = outputPath;
    args.slang = shdc::Slang::bit(outputSlang);
    args.byte_code = false;
    args.output_format = shdc::Format::BARE;
    args.error_format = shdc::ErrMsg::GCC;
    args.no_log_cmdline = true;

    const std::filesystem::path outputPathFs(outputPath);
    const std::filesystem::path outputDirectory = outputPathFs.parent_path();
    if (!outputDirectory.empty()) {
        std::error_code ec;
        std::filesystem::create_directories(outputDirectory, ec);
        if (ec) {
            SetError(errorMessage, "failed to create shader output directory: " + outputDirectory.string());
            return false;
        }
    }

    args.tmpdir = outputDirectory.empty() ? "." : outputDirectory.string();
    if (!args.tmpdir.empty() && args.tmpdir.back() != '/' && args.tmpdir.back() != '\\') {
        args.tmpdir += "/";
    }

    SpirvToolProcessScope spirvScope;

    shdc::Input inp = shdc::Input::load_and_parse(args.input, args.module);
    if (inp.out_error.valid()) {
        SetError(errorMessage, inp.out_error.as_string(args.error_format));
        return false;
    }

    std::array<shdc::Spirv, shdc::Slang::Num> spirv;
    for (int i = 0; i < shdc::Slang::Num; ++i) {
        const auto targetSlang = shdc::Slang::from_index(i);
        if ((args.slang & shdc::Slang::bit(targetSlang)) == 0) {
            continue;
        }

        spirv[i] = shdc::Spirv::compile_glsl_and_extract_bindings(inp, targetSlang, args.defines);
        if (HasErrorMessages(spirv[i].errors, args.error_format, errorMessage)) {
            return false;
        }
    }

    std::array<shdc::Spirvcross, shdc::Slang::Num> spirvcross;
    for (int i = 0; i < shdc::Slang::Num; ++i) {
        const auto targetSlang = shdc::Slang::from_index(i);
        if ((args.slang & shdc::Slang::bit(targetSlang)) == 0) {
            continue;
        }

        spirvcross[i] = shdc::Spirvcross::translate(inp, spirv[i], targetSlang);
        if (spirvcross[i].error.valid()) {
            SetError(errorMessage, spirvcross[i].error.as_string(args.error_format));
            return false;
        }
    }

    std::array<shdc::Bytecode, shdc::Slang::Num> bytecode;
    for (int i = 0; i < shdc::Slang::Num; ++i) {
        const auto targetSlang = shdc::Slang::from_index(i);
        if ((args.slang & shdc::Slang::bit(targetSlang)) == 0) {
            continue;
        }
        if (!args.byte_code && !shdc::Slang::is_spirv(targetSlang)) {
            continue;
        }

        bytecode[i] = shdc::Bytecode::compile(args, inp, spirvcross[i], targetSlang);
        if (HasErrorMessages(bytecode[i].errors, args.error_format, errorMessage)) {
            return false;
        }
    }

    const shdc::refl::Reflection reflection = shdc::refl::Reflection::build(args, inp, spirvcross);
    if (reflection.error.valid()) {
        SetError(errorMessage, reflection.error.as_string(args.error_format));
        return false;
    }

    const shdc::gen::GenInput genInput(args, inp, spirvcross, bytecode, reflection);
    const shdc::ErrMsg generateError = shdc::gen::generate(args.output_format, genInput);
    if (generateError.valid()) {
        SetError(errorMessage, generateError.as_string(args.error_format));
        return false;
    }

    if (compiledShaderInfo != nullptr) {
        *compiledShaderInfo = {};
        if (!PopulateCompiledShaderInfo(outputPath, inp, outputSlang, reflection, *compiledShaderInfo, errorMessage)) {
            return false;
        }
    }

    return true;
}
