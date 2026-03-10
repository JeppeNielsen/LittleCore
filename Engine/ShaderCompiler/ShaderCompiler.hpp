//
// Created by Jeppe Nielsen on 12/02/2024.
//

#pragma once
#include <string>

namespace LittleCore {
    class ShaderCompiler {
    public:
        static bool CompileSokolBare(const std::string& inputPath,
                                     const std::string& outputPath,
                                     const std::string& slang,
                                     std::string* errorMessage = nullptr);
    };
}
