//
// Created by Jeppe Nielsen on 12/02/2024.
//

#include "ShaderCompiler.hpp"
#include "ShaderParser.hpp"
#include "FileHelper.hpp"

using namespace LittleCore;

struct FileCleanUp {
    FileCleanUp(const ShaderCompilerSettings& settings) : settings(settings) {}

    const ShaderCompilerSettings& settings;

    ~FileCleanUp() {
        FileHelper::TryDeleteFile(settings.varyingsWorkingPath);
        FileHelper::TryDeleteFile(settings.vertexWorkingPath);
        FileHelper::TryDeleteFile(settings.fragmentWorkingPath);
    }
};


bool ShaderCompiler::Compile(const ShaderCompilerSettings &settings) {

    if (settings.shaderInputPath == "") {
        return false;
    }

    FileCleanUp fileCleanUp(settings);

    std::string shaderSource = FileHelper::ReadAllText(settings.shaderInputPath);

    if (shaderSource == "") {
        return false;
    }

    ShaderParser parser;

    auto parserResult = parser.TryParse(shaderSource);

    if (!parserResult.succes) {
        return false;
    }

    if (!FileHelper::TryWriteAllText(settings.varyingsWorkingPath, parserResult.varyings)) {
        return false;
    }

    if (!FileHelper::TryWriteAllText(settings.vertexWorkingPath, parserResult.vertex)) {
        return false;
    }

    if (!FileHelper::TryWriteAllText(settings.fragmentWorkingPath, parserResult.fragment)) {
        return false;
    }

    // Temporary migration behavior: write parsed shader stages directly until
    // the sokol-shdc integration replaces the previous sokol shader compiler path.
    if (!FileHelper::TryWriteAllText(settings.vertexOutputPath, parserResult.vertex)) {
        return false;
    }
    if (!FileHelper::TryWriteAllText(settings.fragmentOutputPath, parserResult.fragment)) {
        return false;
    }

    return true;
}


