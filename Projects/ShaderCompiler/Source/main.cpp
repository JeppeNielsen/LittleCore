#include <iostream>
#include "ShaderCompiler.hpp"

using namespace LittleCore;

int main() {
    std::string errorMessage;
    if (ShaderCompiler::CompileSokolBare(
            "/Users/jeppe/Jeppes/LittleCore/Projects/TestRendering/Assets/Shaders/TestShader.shader",
            "/Users/jeppe/Jeppes/LittleCore/Projects/ShaderCompiler/Shaders/Temp/compiled",
            "metal_macos",
            nullptr,
            &errorMessage)) {
        std::cout << "Compilation success!\n";
    } else {
        std::cout << "Compilation failed: " << errorMessage << "\n";
    }

    return 0;
}
