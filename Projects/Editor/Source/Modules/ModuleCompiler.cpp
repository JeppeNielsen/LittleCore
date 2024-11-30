//
// Created by Jeppe Nielsen on 13/01/2024.
//

#include "ModuleCompiler.hpp"
#include <cstdlib>
#include <future>
#include <iostream>
#include <sstream>
#include "../CommandRunner/CommandRunner.hpp"

ModuleCompiler::ModuleCompiler(const std::string &clangPath) : clangPath(clangPath){

}

ModuleCompilerResult ModuleCompiler::Compile(const ModuleCompilerContext &context) const {
    std::string clang = clangPath + " ";

    for (auto cppFile : context.cppFiles) {
        clang += cppFile + " ";
    }

    if (!context.hppFiles.empty()) {
        clang += " ";
        for (auto hppFile: context.hppFiles) {
            clang += "-I" + hppFile + " ";
        }
    }

    clang += "-isysroot `xcrun -sdk macosx --show-sdk-path` ";
    clang += "-std=c++17 -dynamiclib -Wno-return-type-c-linkage -O0 ";
    clang += "-o " + context.outputPath + " ";

    std::cout << clang << std::endl;

    std::stringstream ss;

    CommandRunner commandRunner;

    auto commandResult = commandRunner.Run(clang);

    //int ret = std::system(clang.c_str());
    ModuleCompilerResult result;

    if (commandResult.returnCode != 0) {
        result.errors.push_back("Compilation failed");
    } else {
        result.errors.push_back("Compilation succeeded");
    }
    result.errors.push_back(commandResult.result);
    return result;
}
