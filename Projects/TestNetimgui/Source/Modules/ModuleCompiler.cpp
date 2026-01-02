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

    if (!context.defines.empty()) {
        clang += " ";
        for (auto define: context.defines) {
            clang += "-D" + define + " ";
        }
    }

    if (!context.libPaths.empty()) {
        clang += " ";
        for (auto libPath: context.libPaths) {
            clang += "-L" + libPath + " ";
        }
    }

    if (!context.libs.empty()) {
        clang += " ";
        for (auto lib: context.libs) {
            clang += "-l" + lib + " ";
        }
    }

    if (!context.frameworks.empty()) {
        clang += " ";
        for (auto framework: context.frameworks) {
            clang += "-framework " + framework + " ";
        }
    }

    std::string clangRoot = "/Users/jeppe/Jeppes/Scripting/clang18";

    clang += "-isysroot `xcrun -sdk macosx --show-sdk-path` ";
    clang += "-std=c++23 -Wno-return-type-c-linkage -O0 ";
    clang += "-arch arm64 ";
    clang += "-mmacosx-version-min=13.0 ";
    clang += "-nostdinc++ -isystem "+clangRoot+"/include/c++/v1 ";
    clang += "-L\""+clangRoot+"/lib\" -Wl,-rpath,\""+clangRoot+"/lib\" ";
    clang += "-lc++ -lc++abi ";
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
