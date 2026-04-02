//
// Created by Jeppe Nielsen on 20/03/2026.
//

#include "ProgramDefinition.hpp"
#include "ProgramCompiler.hpp"
#include "../Project/TargetProjectSettings.hpp"
#include "FileHelper.hpp"
#include <filesystem>
#include <sstream>

namespace {
    std::string EscapeForStringLiteral(const std::string& value) {
        std::string escaped;
        escaped.reserve(value.size());

        for (const char c : value) {
            if (c == '\\' || c == '"') {
                escaped += '\\';
            }
            escaped += c;
        }

        return escaped;
    }

    std::string EnsureTrailingSlash(const std::string& value) {
        if (value.empty() || value.back() == '/' || value.back() == '\\') {
            return value;
        }
        return value + "/";
    }
}

ProgramDefinition::ProgramDefinition(const std::string& rootPath,
                                     const std::string& engineAssetsPath,
                                     const std::string& cachePath,
                                     const ProgramCompilerContext& sharedContext,
                                     const DiscoveredProgram& program) :
        rootPath(std::filesystem::path(rootPath).lexically_normal().generic_string()),
        engineAssetsPath(EnsureTrailingSlash(std::filesystem::path(engineAssetsPath).lexically_normal().generic_string())),
        cachePath(std::filesystem::path(cachePath).lexically_normal().generic_string()),
        sharedContext(sharedContext),
        id(program.id),
        stateTypeName(program.stateTypeName),
        sourcePath(std::filesystem::path(program.sourcePath).lexically_normal().generic_string()) {
}

const std::string& ProgramDefinition::Id() const {
    return id;
}

const std::string& ProgramDefinition::StateTypeName() const {
    return stateTypeName;
}

const std::string& ProgramDefinition::SourcePath() const {
    return sourcePath;
}

std::string ProgramDefinition::GeneratedMainPath() const {
    return (std::filesystem::path(cachePath) / (id + ".generated.cpp")).generic_string();
}

std::string ProgramDefinition::ExecutablePath() const {
    return (std::filesystem::path(cachePath) / id).generic_string();
}

bool ProgramDefinition::ExecutableExists() const {
    return std::filesystem::exists(ExecutablePath());
}

std::string ProgramDefinition::CreateGeneratedMainSource() const {
    std::ostringstream source;
    source << "#include \"Engine.hpp\"\n";
    source << "#include \"" << EscapeForStringLiteral(sourcePath) << "\"\n\n";
    source << "int main() {\n";
    source << "    using namespace LittleCore;\n";
    source << "    Engine engine({\n";
    source << "        .mainWindowTitle = \"" << EscapeForStringLiteral(id) << "\",\n";
    source << "        .showWindow = true\n";
    source << "    });\n";
    source << "    engine.Start<" << stateTypeName << ">([](" << stateTypeName << "& state) {\n";
    source << "        state.context.name = \"" << EscapeForStringLiteral(id) << "\";\n";
    source << "        state.context.projectRoot = \"" << EscapeForStringLiteral(rootPath) << "\";\n";
    source << "        state.context.engineRoot = \"" << EscapeForStringLiteral(engineAssetsPath) << "\";\n";
    source << "    });\n";
    source << "    return 0;\n";
    source << "}\n";
    return source.str();
}

void ProgramDefinition::EnsureGeneratedMainFile() const {
    std::filesystem::create_directories(std::filesystem::path(cachePath));
    auto contents = CreateGeneratedMainSource();
    const auto generatedMainPath = GeneratedMainPath();
    if (LittleCore::FileHelper::FileExists(generatedMainPath) &&
        LittleCore::FileHelper::ReadAllText(generatedMainPath) == contents) {
        return;
    }

    LittleCore::FileHelper::TryWriteAllText(generatedMainPath, contents);
}

ProgramCompilerResult ProgramDefinition::Build() const {
    EnsureGeneratedMainFile();

    ProgramCompilerContext context = sharedContext;
    context.sourceFiles.push_back(GeneratedMainPath());
    context.outputPath = ExecutablePath();

    ProgramCompiler compiler("/Users/jeppe/Jeppes/Scripting/clang18/bin/clang++");
    return compiler.Compile(context);
}
