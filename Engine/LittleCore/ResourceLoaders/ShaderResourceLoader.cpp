//
// Created by Jeppe Nielsen on 03/03/2024.
//

#include "ShaderResourceLoader.hpp"
#include "../../ShaderCompiler/ShaderCompiler.hpp"
#include "FileHelper.hpp"

using namespace LittleCore;
using namespace bgfx;

void ClearVector(void* ptr, void* userData) {
    std::vector<u_int8_t>* v = (std::vector<u_int8_t>*)userData;
    v->clear();
}

void ShaderResourceLoader::Load(ShaderResource& resource) {

    ShaderCompilerSettings settings;
    settings.platform = ShaderPlatform::OSX;
    settings.shaderInputPath = path;
    settings.includePath = "/Users/jeppe/Jeppes/LittleCore/External/bgfx/src/"; // TODO: fix hardcoded path

    settings.varyingsWorkingPath = path + "_varyings.temp";
    settings.vertexWorkingPath = path + "_vertex.temp";
    settings.fragmentWorkingPath = path + "_fragment.temp";

    settings.vertexOutputPath = path +".vertex";
    settings.fragmentOutputPath = path +".fragment";
    ShaderCompiler shaderCompiler;
    shaderCompiler.Compile(settings);

    vertexShader = FileHelper::ReadData(settings.vertexOutputPath);
    fragmentShader = FileHelper::ReadData(settings.fragmentOutputPath);

    const bgfx::Memory* vsSource = bgfx::makeRef(&vertexShader[0], vertexShader.size(), ClearVector, &vertexShader);
    bgfx::ShaderHandle vsShader = bgfx::createShader(vsSource);

    const bgfx::Memory* fsSource = bgfx::makeRef(&fragmentShader[0], fragmentShader.size(), ClearVector, &fragmentShader);
    bgfx::ShaderHandle fsShader = bgfx::createShader(fsSource);

    resource.handle = bgfx::createProgram(vsShader, fsShader, true);
}

void ShaderResourceLoader::Unload(ShaderResource& resource) {
    if (resource.handle.idx != bgfx::kInvalidHandle) {
        bgfx::destroy(resource.handle);
        resource.handle = BGFX_INVALID_HANDLE;
    }
}

bool ShaderResourceLoader::IsLoaded() {
    return true;
}

void ShaderResourceLoader::Reload(ShaderResource& resource) {
    Unload(resource);
    Load(resource);
}
