//
// Created by Jeppe Nielsen on 20/03/2026.
//

#include "ProgramCompiler.hpp"
#include <filesystem>
#include <sstream>
#include "../CommandRunner/CommandRunner.hpp"

namespace {
    std::string QuoteForShell(const std::string& value) {
        std::string quoted = "\"";
        for (const char c : value) {
            if (c == '\\' || c == '"' || c == '$' || c == '`') {
                quoted += '\\';
            }
            quoted += c;
        }
        quoted += "\"";
        return quoted;
    }

    void AppendPrefixedPaths(std::ostringstream& command, const std::vector<std::string>& values, const std::string& prefix) {
        for (const auto& value : values) {
            command << " " << prefix << QuoteForShell(value);
        }
    }
}

ProgramCompiler::ProgramCompiler(std::string clangPath) : clangPath(std::move(clangPath)) {
}

ProgramCompilerResult ProgramCompiler::Compile(const ProgramCompilerContext& context) const {
    std::filesystem::path outputPath(context.outputPath);
    if (outputPath.has_parent_path()) {
        std::filesystem::create_directories(outputPath.parent_path());
    }

    std::ostringstream command;
    command << QuoteForShell(clangPath);

    for (const auto& sourceFile : context.sourceFiles) {
        command << " " << QuoteForShell(sourceFile);
    }

    AppendPrefixedPaths(command, context.includePaths, "-I");
    AppendPrefixedPaths(command, context.defines, "-D");
    AppendPrefixedPaths(command, context.libraryPaths, "-L");

    for (const auto& library : context.libraries) {
        command << " -l" << library;
    }

    for (const auto& framework : context.frameworks) {
        command << " -framework " << framework;
    }

    const std::string clangRoot = "/Users/jeppe/Jeppes/Scripting/clang18";

    command << " -isysroot `xcrun -sdk macosx --show-sdk-path`";
    command << " -std=c++23 -Wno-return-type-c-linkage -O0";
    command << " -arch arm64";
    command << " -mmacosx-version-min=13.0";
    command << " -nostdinc++ -isystem " << QuoteForShell(clangRoot + "/include/c++/v1");
    command << " -L" << QuoteForShell(clangRoot + "/lib");
    command << " -Xlinker -rpath -Xlinker " << QuoteForShell(clangRoot + "/lib");
    command << " -lc++ -lc++abi";
    command << " -o " << QuoteForShell(context.outputPath);

    CommandRunner commandRunner;
    const auto commandString = command.str();
    const auto commandResult = commandRunner.Run(commandString);

    ProgramCompilerResult result;
    result.command = commandString;
    result.succeeded = commandResult.returnCode == 0;
    result.summary = result.succeeded ? "Compilation succeeded" : "Compilation failed";
    result.output = commandResult.result;
    return result;
}
