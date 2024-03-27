//
// Created by Jeppe Nielsen on 13/01/2024.
//

#include "ModuleCompiler.hpp"
#include <cstdlib>
#include <future>
#include <iostream>
#include <sstream>

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
    clang += "-std=c++11 -dynamiclib -Wno-return-type-c-linkage ";
    clang += "-o " + context.outputPath + " ";

    std::cout << clang << std::endl;

    std::stringstream ss;

    int ret = std::system(clang.c_str());
    ModuleCompilerResult result;

    if (ret != 0) {
        result.errors.push_back("Compilation failed");
        result.errors.push_back(ss.str());
    }
    return result;
}
