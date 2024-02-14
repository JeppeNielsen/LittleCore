
#include <iostream>
#include "ShaderCompiler.hpp"
#include "ShaderCompilerSettings.hpp"

using namespace LittleCore;

int main() {

    ShaderCompilerSettings settings;
    settings.platform = ShaderPlatform::OSX;
    settings.shaderInputPath = "/Users/jeppe/Jeppes/LittleCore/Projects/ShaderCompiler/Shaders/TestShader.shader";
    settings.includePath = "/Users/jeppe/Jeppes/LittleCore/External/bgfx/src/";

    settings.varyingsWorkingPath = "/Users/jeppe/Jeppes/LittleCore/Projects/ShaderCompiler/Shaders/Temp/varyings.temp";
    settings.vertexWorkingPath = "/Users/jeppe/Jeppes/LittleCore/Projects/ShaderCompiler/Shaders/Temp/vertex.temp";
    settings.fragmentWorkingPath = "/Users/jeppe/Jeppes/LittleCore/Projects/ShaderCompiler/Shaders/Temp/fragment.temp";

    settings.vertexOutputPath = "/Users/jeppe/Jeppes/LittleCore/Projects/ShaderCompiler/Shaders/TestShader.vertex";
    settings.fragmentOutputPath = "/Users/jeppe/Jeppes/LittleCore/Projects/ShaderCompiler/Shaders/TestShader.fragment";

    ShaderCompiler compiler;

    if (compiler.Compile(settings)) {
        std::cout << "Compilation success!\n";
    } else {
        std::cout << "Compilation failed\n";
    }

    return 0;
}
