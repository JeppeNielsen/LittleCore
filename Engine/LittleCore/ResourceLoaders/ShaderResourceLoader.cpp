//
// Created by Jeppe Nielsen on 03/03/2024.
//

#include "ShaderResourceLoader.hpp"

#include <glm/glm.hpp>

#include <algorithm>
#include <array>
#include <filesystem>
#include <iostream>
#include <regex>
#include <string>

#include "FileHelper.hpp"
#include "ShaderCompiler.hpp"

using namespace LittleCore;

namespace {
    constexpr const char* kViewProjUniformName = "u_modelViewProj";
    constexpr const char* kDefaultTextureName = "colorTexture";

    struct CompiledShaderSource {
        std::string vertexSource;
        std::string fragmentSource;
        std::array<std::string, 3> attributeNames = {"a_position", "a_color0", "a_texcoord0"};
    };

    bool EndsWith(const std::string& value, const std::string& suffix) {
        if (suffix.size() > value.size()) {
            return false;
        }
        return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
    }

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

    std::string GetSlangExtension(const std::string& slang) {
        if (slang.starts_with("metal_")) {
            return ".metal";
        }
        if (slang.starts_with("glsl")) {
            return ".glsl";
        }
        if (slang.starts_with("hlsl")) {
            return ".hlsl";
        }
        if (slang == "wgsl") {
            return ".wgsl";
        }
        return "";
    }

    std::filesystem::path FindCompiledStageFile(const std::filesystem::path& directory,
                                                const std::string& outputBaseName,
                                                const std::string& slang,
                                                const std::string& stageName,
                                                const std::string& extension) {
        const std::string prefix = outputBaseName + "_";
        const std::string suffix = "_" + slang + "_" + stageName + extension;
        std::error_code ec;
        for (const auto& entry : std::filesystem::directory_iterator(directory, ec)) {
            if (ec || !entry.is_regular_file()) {
                continue;
            }
            const std::string filename = entry.path().filename().string();
            if (!filename.starts_with(prefix) || !EndsWith(filename, suffix)) {
                continue;
            }
            return entry.path();
        }
        return {};
    }

    std::string DetectMetalEntryPoint(const std::string& source, const std::string& stageKeyword) {
        const std::array<std::string, 6> knownEntries = {"main0", "_main", "main", "vs_main", "fs_main", "xlatMtlMain"};
        for (const std::string& entry : knownEntries) {
            if (source.find(entry + "(") != std::string::npos) {
                return entry;
            }
        }
        const std::regex stageRegex("\\b" + stageKeyword + R"(\b[^\n\r\(\)]*\s+([A-Za-z_][A-Za-z0-9_]*)\s*\())");
        std::smatch match;
        if (std::regex_search(source, match, stageRegex) && match.size() > 1) {
            return match[1].str();
        }
        return "main";
    }

    std::string DetectEntryPoint(const std::string& source, const std::string& stageKeyword, bool metal) {
        if (metal) {
            return DetectMetalEntryPoint(source, stageKeyword);
        }
        if (source.find("main0(") != std::string::npos) {
            return "main0";
        }
        if (source.find("main(") != std::string::npos) {
            return "main";
        }
        if (source.find("vs_main(") != std::string::npos) {
            return "vs_main";
        }
        if (source.find("fs_main(") != std::string::npos) {
            return "fs_main";
        }
        return "main";
    }

    bool TryCompileWithSokolShdc(const std::string& resourcePath,
                                 const std::string& shaderSource,
                                 CompiledShaderSource& compiled) {
        const sg_backend backend = sg_query_backend();
        const std::string slang = GetSokolSlangForBackend(backend);
        const std::string extension = GetSlangExtension(slang);
        if (slang.empty() || extension.empty()) {
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

        std::filesystem::path inputPath = cacheDir / "shader_input.glsl";
        std::filesystem::path outputBase = cacheDir / "compiled";

        std::string shaderSourceCopy = shaderSource;
        if (!FileHelper::TryWriteAllText(inputPath.string(), shaderSourceCopy)) {
            std::cerr << "ShaderResourceLoader: failed to write shader source '" << inputPath.string() << "'\n";
            return false;
        }

        std::string compileError;
        if (!ShaderCompiler::CompileSokolBare(inputPath.string(), outputBase.string(), slang, &compileError)) {
            std::cerr << "ShaderResourceLoader: sokol shader compile failed for '" << resourcePath << "'"
                      << (compileError.empty() ? "" : (": " + compileError))
                      << ".\n";
            return false;
        }

        const std::filesystem::path vertexPath = FindCompiledStageFile(cacheDir, outputBase.filename().string(), slang, "vertex", extension);
        const std::filesystem::path fragmentPath = FindCompiledStageFile(cacheDir, outputBase.filename().string(), slang, "fragment", extension);
        if (vertexPath.empty() || fragmentPath.empty()) {
            std::cerr << "ShaderResourceLoader: sokol-shdc output missing stage files for '" << resourcePath << "'\n";
            return false;
        }

        compiled.vertexSource = FileHelper::ReadAllText(vertexPath.string());
        compiled.fragmentSource = FileHelper::ReadAllText(fragmentPath.string());
        if (compiled.vertexSource.empty() || compiled.fragmentSource.empty()) {
            std::cerr << "ShaderResourceLoader: empty compiled shader output for '" << resourcePath << "'\n";
            return false;
        }
        return true;
    }

    bool TryCreateCompiledShaderSource(const std::string& resourcePath, CompiledShaderSource& compiled) {
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

        return TryCompileWithSokolShdc(resourcePath, shaderFile, compiled);
    }

    sg_shader CreateShader(const CompiledShaderSource& compiledSource) {
        sg_shader_desc desc = {};
        desc.attrs[0].glsl_name = compiledSource.attributeNames[0].c_str();
        desc.attrs[1].glsl_name = compiledSource.attributeNames[1].c_str();
        desc.attrs[2].glsl_name = compiledSource.attributeNames[2].c_str();

        desc.vertex_func.source = compiledSource.vertexSource.c_str();
        desc.fragment_func.source = compiledSource.fragmentSource.c_str();

        const sg_backend backend = sg_query_backend();
        const bool metalBackend =
            backend == SG_BACKEND_METAL_MACOS ||
            backend == SG_BACKEND_METAL_IOS ||
            backend == SG_BACKEND_METAL_SIMULATOR;

        const std::string vertexEntry = DetectEntryPoint(compiledSource.vertexSource, "vertex", metalBackend);
        const std::string fragmentEntry = DetectEntryPoint(compiledSource.fragmentSource, "fragment", metalBackend);
        desc.vertex_func.entry = vertexEntry.c_str();
        desc.fragment_func.entry = fragmentEntry.c_str();

        if (backend == SG_BACKEND_D3D11) {
            desc.vertex_func.d3d11_target = "vs_4_0";
            desc.fragment_func.d3d11_target = "ps_4_0";
        }

        sg_shader_uniform_block* uniformBlock = &desc.uniform_blocks[0];
        uniformBlock->stage = SG_SHADERSTAGE_VERTEX;
        uniformBlock->size = sizeof(glm::mat4);
        uniformBlock->layout = SG_UNIFORMLAYOUT_STD140;
        uniformBlock->hlsl_register_b_n = 0;
        uniformBlock->msl_buffer_n = 0;
        uniformBlock->glsl_uniforms[0].glsl_name = kViewProjUniformName;
        uniformBlock->glsl_uniforms[0].type = SG_UNIFORMTYPE_MAT4;
        uniformBlock->glsl_uniforms[0].array_count = 1;

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
    if (path.empty()) {
        return;
    }

    CompiledShaderSource compiled;
    if (!TryCreateCompiledShaderSource(path, compiled)) {
        return;
    }

    resource.handle = CreateShader(compiled);
    const sg_resource_state shaderState = sg_query_shader_state(resource.handle);
    if (shaderState == SG_RESOURCESTATE_FAILED || shaderState == SG_RESOURCESTATE_INVALID) {
        sg_destroy_shader(resource.handle);
        resource.handle = {SG_INVALID_ID};
    }
}

void ShaderResourceLoader::Unload(ShaderResource& resource) {
    if (resource.handle.id != SG_INVALID_ID) {
        sg_destroy_shader(resource.handle);
        resource.handle = {SG_INVALID_ID};
    }
}

bool ShaderResourceLoader::IsLoaded() {
    return true;
}

void ShaderResourceLoader::Reload(ShaderResource& resource) {
    Unload(resource);
    Load(resource);
}
