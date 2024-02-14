//
// Created by Jeppe Nielsen on 12/02/2024.
//

#include "ShaderCompiler.hpp"
#include "ShaderParser.hpp"
#include "FileHelper.hpp"
#include "shaderc.h"

using namespace LittleCore;


bool CompileVertex(const ShaderCompilerSettings& settings) {
    const char *args[14];

    args[0] = "-f";
    args[1] = settings.vertexWorkingPath.c_str(); //"/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/vs_cubes.sc";
    args[2] = "-o";
    args[3] = settings.vertexOutputPath.c_str(); //"/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/vs_cubes.bin";
    args[4] = "--platform";
    args[5] = "osx";
    args[6] = "-p";
    args[7] = "metal";
    args[8] = "--type";
    args[9] = "vertex";
    args[10] = "-i";
    args[11] = settings.includePath.c_str(); //"/Users/jeppe/Jeppes/LittleCore/External/bgfx/src/";
    args[12] = "--varyingdef";
    args[13] = settings.varyingsWorkingPath.c_str(); //"/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/varying.def.sc";
    int res = bgfx::compileShader(14, args);

    return res == bx::kExitSuccess;
}


bool CompileFragment(const ShaderCompilerSettings& settings) {
    const char *args[14];

    args[0] = "-f";
    args[1] = settings.fragmentWorkingPath.c_str(); //"/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/fs_cubes.sc";
    args[2] = "-o";
    args[3] = settings.fragmentOutputPath.c_str(); //"/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/fs_cubes.bin";
    args[4] = "--platform";
    args[5] = "osx";
    args[6] = "-p";
    args[7] = "metal";
    args[8] = "--type";
    args[9] = "fragment";
    args[10] = "-i";
    args[11] = settings.includePath.c_str();//"/Users/jeppe/Jeppes/LittleCore/External/bgfx/src/";
    args[12] = "--varyingdef";
    args[13] = settings.varyingsWorkingPath.c_str();//"/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/varying.def.sc";

    int res = bgfx::compileShader(14, args);
    return res == bx::kExitSuccess;
}

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

    if (!CompileVertex(settings)) {
        return false;
    }
    if (!CompileFragment(settings)) {
        return false;
    }

    return true;
}



