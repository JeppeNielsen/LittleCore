
#include <iostream>
#include "shaderc.h"
#include <vector>
#include <string>
#include "ShaderParser.hpp"
#include "FileReader.hpp"

using namespace LittleCore;

int main() {

    std::string shaderSource = FileReader::ReadAllText("/Users/jeppe/Jeppes/LittleCore/Projects/ShaderCompiler/Source/TestShader.shader");


    ShaderParser parser;

    auto result = parser.TryParse(shaderSource);

    if (!result.succes) {
        std::cout << "Parsing not correct!";
    } else {

        std::cout << "Varyings:\n";
        std::cout << result.varyings << std::endl;

        std::cout << "Vertex:\n";
        std::cout << result.vertex << std::endl;

        std::cout << "Fragment:\n";
        std::cout << result.fragment << std::endl;
    }

    return 0;



   // bgfx::Options options;

    {
        const char *args[14];

        args[0] = "-f";
        args[1] = "/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/fs_cubes.sc";
        args[2] = "-o";
        args[3] = "/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/fs_cubes.bin";
        args[4] = "--platform";
        args[5] = "osx";
        args[6] = "-p";
        args[7] = "metal";
        args[8] = "--type";
        args[9] = "fragment";
        args[10] = "-i";
        args[11] = "/Users/jeppe/Jeppes/LittleCore/External/bgfx/src/";
        args[12] = "--varyingdef";
        args[13] = "/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/varying.def.sc";

        bgfx::compileShader(14, args);
    }

    {
        const char *args[14];

        args[0] = "-f";
        args[1] = "/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/vs_cubes.sc";
        args[2] = "-o";
        args[3] = "/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/vs_cubes.bin";
        args[4] = "--platform";
        args[5] = "osx";
        args[6] = "-p";
        args[7] = "metal";
        args[8] = "--type";
        args[9] = "vertex";
        args[10] = "-i";
        args[11] = "/Users/jeppe/Jeppes/LittleCore/External/bgfx/src/";
        args[12] = "--varyingdef";
        args[13] = "/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/varying.def.sc";
        bgfx::compileShader(14, args);
    }

    //bgfx::compileMetalShader(options, 0, "code", nullptr);

    return 0;
}
