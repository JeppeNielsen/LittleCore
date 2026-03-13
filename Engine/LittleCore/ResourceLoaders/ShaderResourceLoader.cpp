//
// Created by Jeppe Nielsen on 03/03/2024.
//

#include "ShaderResourceLoader.hpp"

#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "CompiledShaderInfo.hpp"
#include "FileHelper.hpp"
#include "ShaderCompiler.hpp"

using namespace LittleCore;

namespace {
    constexpr const char* kDefaultTextureName = "colorTexture";

    bool IsSokolAnnotatedShader(const std::string& shaderSource) {
        return shaderSource.find("@vs") != std::string::npos &&
               shaderSource.find("@fs") != std::string::npos &&
               shaderSource.find("@program") != std::string::npos;
    }

    std::string GetSokolSlangForBackend(const sg_backend backend) {
        switch (backend) {
            case SG_BACKEND_METAL_MACOS:
                return "metal_macos";
            case SG_BACKEND_METAL_IOS:
                return "metal_ios";
            case SG_BACKEND_METAL_SIMULATOR:
                return "metal_sim";
            case SG_BACKEND_GLES3:
                return "glsl300es";
            case SG_BACKEND_GLCORE:
            case SG_BACKEND_DUMMY:
                return "glsl410";
            case SG_BACKEND_D3D11:
                return "hlsl4";
            case SG_BACKEND_WGPU:
                return "wgsl";
            default:
                return "";
        }
    }

    bool TryCreateCompiledShaderSource(const std::string& resourcePath, CompiledShaderInfo& compiled) {
        const std::string shaderFile = FileHelper::ReadAllText(resourcePath);
        if (shaderFile.empty()) {
            std::cerr << "ShaderResourceLoader: failed to read shader file '" << resourcePath << "'\n";
            return false;
        }

        if (!IsSokolAnnotatedShader(shaderFile)) {
            std::cerr << "ShaderResourceLoader: shader file must use sokol-shdc annotations (@vs/@fs/@program): '"
                      << resourcePath << "'\n";
            return false;
        }

        const sg_backend backend = sg_query_backend();
        const std::string slang = GetSokolSlangForBackend(backend);
        if (slang.empty()) {
            std::cerr << "ShaderResourceLoader: unsupported sokol backend for shader '" << resourcePath << "'\n";
            return false;
        }

        std::error_code ec;
        std::filesystem::path cacheDir = std::filesystem::temp_directory_path(ec);
        if (ec) {
            std::cerr << "ShaderResourceLoader: failed to resolve temp directory for shader '" << resourcePath << "'\n";
            return false;
        }
        cacheDir /= "littlecore_sokol_shader_cache";
        cacheDir /= std::to_string(std::hash<std::string>{}(resourcePath));
        std::filesystem::create_directories(cacheDir, ec);
        if (ec) {
            std::cerr << "ShaderResourceLoader: failed to create shader cache directory '" << cacheDir.string() << "'\n";
            return false;
        }

        const std::filesystem::path inputPath = cacheDir / "shader_input.glsl";
        const std::filesystem::path outputBase = cacheDir / "compiled";

        std::string shaderSourceCopy = shaderFile;
        if (!FileHelper::TryWriteAllText(inputPath.string(), shaderSourceCopy)) {
            std::cerr << "ShaderResourceLoader: failed to write shader source '" << inputPath.string() << "'\n";
            return false;
        }

        std::string compileError;
        if (!ShaderCompiler::CompileSokolBare(inputPath.string(), outputBase.string(), slang, &compiled, &compileError)) {
            std::cerr << "ShaderResourceLoader: sokol shader compile failed for '" << resourcePath << "'"
                      << (compileError.empty() ? "" : (": " + compileError))
                      << ".\n";
            return false;
        }

        return true;
    }

    std::vector<LittleCore::ShaderUniformBlockInfo> CreateRuntimeUniformBlocks(const CompiledShaderInfo& compiledSource) {
        std::vector<LittleCore::ShaderUniformBlockInfo> uniformBlocks;
        uniformBlocks.reserve(compiledSource.uniformBlocks.size());

        for (const auto& compiledBlock : compiledSource.uniformBlocks) {
            LittleCore::ShaderUniformBlockInfo runtimeBlock;
            runtimeBlock.slot = compiledBlock.slot;
            runtimeBlock.stage = compiledBlock.stage;
            runtimeBlock.layout = compiledBlock.layout;
            runtimeBlock.size = compiledBlock.size;
            runtimeBlock.uniforms.reserve(compiledBlock.uniforms.size());

            for (const auto& compiledUniform : compiledBlock.uniforms) {
                LittleCore::ShaderUniformInfo runtimeUniform;
                runtimeUniform.name = compiledUniform.name;
                runtimeUniform.type = compiledUniform.type;
                runtimeUniform.arrayCount = compiledUniform.arrayCount;
                runtimeBlock.uniforms.push_back(std::move(runtimeUniform));
            }

            uniformBlocks.push_back(std::move(runtimeBlock));
        }

        return uniformBlocks;
    }

    sg_shader CreateShader(const CompiledShaderInfo& compiledSource) {
        sg_shader_desc desc = {};
        desc.attrs[0].glsl_name = compiledSource.attributeNames[0].c_str();
        desc.attrs[1].glsl_name = compiledSource.attributeNames[1].c_str();
        desc.attrs[2].glsl_name = compiledSource.attributeNames[2].c_str();

        desc.vertex_func.source = compiledSource.vertexShader.source.c_str();
        desc.fragment_func.source = compiledSource.fragmentShader.source.c_str();
        desc.vertex_func.entry = compiledSource.vertexShader.entryPoint.c_str();
        desc.fragment_func.entry = compiledSource.fragmentShader.entryPoint.c_str();

        if (sg_query_backend() == SG_BACKEND_D3D11) {
            desc.vertex_func.d3d11_target = "vs_4_0";
            desc.fragment_func.d3d11_target = "ps_4_0";
        }

        for (const auto& uniformBlockInfo : compiledSource.uniformBlocks) {
            sg_shader_uniform_block* uniformBlock = &desc.uniform_blocks[uniformBlockInfo.slot];
            uniformBlock->stage = uniformBlockInfo.stage;
            uniformBlock->size = uniformBlockInfo.size;
            uniformBlock->layout = uniformBlockInfo.layout;

            if (uniformBlockInfo.hlslRegisterBN >= 0) {
                uniformBlock->hlsl_register_b_n = static_cast<uint8_t>(uniformBlockInfo.hlslRegisterBN);
            }
            if (uniformBlockInfo.mslBufferN >= 0) {
                uniformBlock->msl_buffer_n = static_cast<uint8_t>(uniformBlockInfo.mslBufferN);
            }
            if (uniformBlockInfo.wgslGroup0BindingN >= 0) {
                uniformBlock->wgsl_group0_binding_n = static_cast<uint8_t>(uniformBlockInfo.wgslGroup0BindingN);
            }

            for (std::size_t uniformIndex = 0; uniformIndex < uniformBlockInfo.glslUniforms.size(); ++uniformIndex) {
                uniformBlock->glsl_uniforms[uniformIndex].glsl_name = uniformBlockInfo.glslUniforms[uniformIndex].glslName.c_str();
                uniformBlock->glsl_uniforms[uniformIndex].type = uniformBlockInfo.glslUniforms[uniformIndex].type;
                uniformBlock->glsl_uniforms[uniformIndex].array_count = uniformBlockInfo.glslUniforms[uniformIndex].arrayCount;
            }
        }

        sg_shader_image* image = &desc.images[0];
        image->stage = SG_SHADERSTAGE_FRAGMENT;
        image->image_type = SG_IMAGETYPE_2D;
        image->sample_type = SG_IMAGESAMPLETYPE_FLOAT;
        image->hlsl_register_t_n = 0;
        image->msl_texture_n = 0;

        sg_shader_sampler* sampler = &desc.samplers[0];
        sampler->stage = SG_SHADERSTAGE_FRAGMENT;
        sampler->sampler_type = SG_SAMPLERTYPE_FILTERING;
        sampler->hlsl_register_s_n = 0;
        sampler->msl_sampler_n = 0;

        sg_shader_image_sampler_pair* pair = &desc.image_sampler_pairs[0];
        pair->stage = SG_SHADERSTAGE_FRAGMENT;
        pair->image_slot = 0;
        pair->sampler_slot = 0;
        pair->glsl_name = kDefaultTextureName;

        return sg_make_shader(desc);
    }
}

void ShaderResourceLoader::Load(ShaderResource& resource) {
    resource.handle = {SG_INVALID_ID};
    resource.uniformBlocks.clear();
    if (path.empty()) {
        return;
    }

    CompiledShaderInfo compiled;
    if (!TryCreateCompiledShaderSource(path, compiled)) {
        return;
    }

    resource.handle = CreateShader(compiled);
    resource.uniformBlocks = CreateRuntimeUniformBlocks(compiled);
    const sg_resource_state shaderState = sg_query_shader_state(resource.handle);
    if (shaderState == SG_RESOURCESTATE_FAILED || shaderState == SG_RESOURCESTATE_INVALID) {
        sg_destroy_shader(resource.handle);
        resource.handle = {SG_INVALID_ID};
        resource.uniformBlocks.clear();
    }
}

void ShaderResourceLoader::Unload(ShaderResource& resource) {
    if (resource.handle.id != SG_INVALID_ID) {
        sg_destroy_shader(resource.handle);
        resource.handle = {SG_INVALID_ID};
    }
    resource.uniformBlocks.clear();
}

bool ShaderResourceLoader::IsLoaded() {
    return true;
}

void ShaderResourceLoader::Reload(ShaderResource& resource) {
    Unload(resource);
    Load(resource);
}
