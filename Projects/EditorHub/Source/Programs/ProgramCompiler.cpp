//
// Created by Jeppe Nielsen on 20/03/2026.
//

#include "ProgramCompiler.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string_view>
#include <system_error>
#include <vector>
#include "../CommandRunner/CommandRunner.hpp"

namespace {
    constexpr std::string_view CompilerVersion = "ProgramCompiler-v2";
    constexpr std::string_view ClangRoot = "/Users/jeppe/Jeppes/Scripting/clang18";

    struct SourceBuildPaths {
        std::filesystem::path objectPath;
        std::filesystem::path dependencyPath;
        std::filesystem::path signaturePath;
    };

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

    std::string TrimWhitespace(std::string value) {
        const auto isWhitespace = [](unsigned char c) {
            return std::isspace(c) != 0;
        };

        while (!value.empty() && isWhitespace(static_cast<unsigned char>(value.front()))) {
            value.erase(value.begin());
        }

        while (!value.empty() && isWhitespace(static_cast<unsigned char>(value.back()))) {
            value.pop_back();
        }

        return value;
    }

    uint64_t HashTokenized(std::initializer_list<std::string_view> values) {
        constexpr uint64_t offset = 14695981039346656037ull;
        constexpr uint64_t prime = 1099511628211ull;

        uint64_t hash = offset;
        for (const auto value : values) {
            for (const unsigned char c : value) {
                hash ^= c;
                hash *= prime;
            }
            hash ^= 0xff;
            hash *= prime;
        }
        return hash;
    }

    void AppendHashValue(uint64_t& hash, const std::string_view value) {
        constexpr uint64_t prime = 1099511628211ull;
        for (const unsigned char c : value) {
            hash ^= c;
            hash *= prime;
        }
        hash ^= 0xff;
        hash *= prime;
    }

    std::string HashToHex(uint64_t value) {
        std::ostringstream stream;
        stream << std::hex << value;
        return stream.str();
    }

    std::string SanitizeFileComponent(std::string value) {
        for (char& c : value) {
            const auto ch = static_cast<unsigned char>(c);
            if (!std::isalnum(ch) && c != '_' && c != '-') {
                c = '_';
            }
        }
        return value;
    }

    const std::string& ResolveSdkPath() {
        static const std::string sdkPath = []() {
            CommandRunner commandRunner;
            const auto result = commandRunner.Run("xcrun -sdk macosx --show-sdk-path");
            if (result.returnCode != 0) {
                return std::string();
            }
            return TrimWhitespace(result.result);
        }();

        return sdkPath;
    }

    std::string ReadTextFile(const std::filesystem::path& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            return {};
        }

        std::ostringstream contents;
        contents << file.rdbuf();
        return contents.str();
    }

    bool WriteTextFile(const std::filesystem::path& path, const std::string& contents) {
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }

        file << contents;
        return file.good();
    }

    std::string BuildCompileSignature(const ProgramCompilerContext& context,
                                      const std::string& clangPath,
                                      const std::string& sdkPath,
                                      const std::string& sourceFile) {
        uint64_t hash = HashTokenized({
                CompilerVersion,
                clangPath,
                sdkPath,
                ClangRoot,
                sourceFile,
                "-std=c++23",
                "-Wno-return-type-c-linkage",
                "-O0",
                "-g",
                "-arch arm64",
                "-mmacosx-version-min=13.0",
                "-nostdinc++"
        });

        for (const auto& includePath : context.includePaths) {
            AppendHashValue(hash, includePath);
        }

        for (const auto& define : context.defines) {
            AppendHashValue(hash, define);
        }

        return HashToHex(hash);
    }

    std::string BuildLinkSignature(const ProgramCompilerContext& context,
                                   const std::string& clangPath,
                                   const std::string& sdkPath,
                                   const std::vector<std::string>& objectFiles) {
        uint64_t hash = HashTokenized({
                CompilerVersion,
                "link",
                clangPath,
                sdkPath,
                ClangRoot,
                "-arch arm64",
                "-mmacosx-version-min=13.0"
        });

        for (const auto& objectFile : objectFiles) {
            AppendHashValue(hash, objectFile);
        }

        for (const auto& libraryPath : context.libraryPaths) {
            AppendHashValue(hash, libraryPath);
        }

        for (const auto& library : context.libraries) {
            AppendHashValue(hash, library);
        }

        for (const auto& framework : context.frameworks) {
            AppendHashValue(hash, framework);
        }

        return HashToHex(hash);
    }

    SourceBuildPaths CreateSourceBuildPaths(const std::filesystem::path& outputPath, const std::string& sourceFile) {
        const auto sourceName = std::filesystem::path(sourceFile).filename().string();
        const auto sourceHash = HashToHex(HashTokenized({sourceFile}));
        const auto baseName = SanitizeFileComponent(sourceName) + "_" + sourceHash;
        const auto objectDirectory = outputPath.parent_path() / "Objects" / outputPath.filename();

        return {
                objectDirectory / (baseName + ".o"),
                objectDirectory / (baseName + ".d"),
                objectDirectory / (baseName + ".sig")
        };
    }

    std::vector<std::string> ParseDependencyList(std::string contents) {
        while (true) {
            const auto continuation = contents.find("\\\n");
            if (continuation == std::string::npos) {
                break;
            }
            contents.erase(continuation, 2);
        }

        const auto colon = contents.find(':');
        if (colon == std::string::npos) {
            return {};
        }

        std::vector<std::string> dependencies;
        std::string current;
        bool escapeNext = false;

        for (size_t i = colon + 1; i < contents.size(); ++i) {
            const char c = contents[i];
            if (escapeNext) {
                current += c;
                escapeNext = false;
                continue;
            }

            if (c == '\\') {
                escapeNext = true;
                continue;
            }

            if (std::isspace(static_cast<unsigned char>(c)) != 0) {
                if (!current.empty()) {
                    dependencies.push_back(current);
                    current.clear();
                }
                continue;
            }

            current += c;
        }

        if (!current.empty()) {
            dependencies.push_back(current);
        }

        return dependencies;
    }

    bool NeedsRecompile(const std::string& sourceFile,
                        const SourceBuildPaths& paths,
                        const std::string& expectedSignature) {
        std::error_code errorCode;
        if (!std::filesystem::exists(paths.objectPath, errorCode) ||
            !std::filesystem::exists(paths.dependencyPath, errorCode) ||
            !std::filesystem::exists(paths.signaturePath, errorCode)) {
            return true;
        }

        if (ReadTextFile(paths.signaturePath) != expectedSignature) {
            return true;
        }

        auto dependencies = ParseDependencyList(ReadTextFile(paths.dependencyPath));
        if (dependencies.empty()) {
            return true;
        }

        if (std::find(dependencies.begin(), dependencies.end(), sourceFile) == dependencies.end()) {
            dependencies.push_back(sourceFile);
        }

        const auto objectWriteTime = std::filesystem::last_write_time(paths.objectPath, errorCode);
        if (errorCode) {
            return true;
        }

        for (const auto& dependency : dependencies) {
            const std::filesystem::path dependencyPath(dependency);
            if (!std::filesystem::exists(dependencyPath, errorCode)) {
                return true;
            }

            const auto dependencyWriteTime = std::filesystem::last_write_time(dependencyPath, errorCode);
            if (errorCode || dependencyWriteTime > objectWriteTime) {
                return true;
            }
        }

        return false;
    }

    bool NeedsLink(const std::filesystem::path& outputPath,
                   const std::filesystem::path& linkSignaturePath,
                   const std::string& expectedSignature,
                   const std::vector<std::string>& objectFiles) {
        std::error_code errorCode;
        if (!std::filesystem::exists(outputPath, errorCode) ||
            !std::filesystem::exists(linkSignaturePath, errorCode)) {
            return true;
        }

        if (ReadTextFile(linkSignaturePath) != expectedSignature) {
            return true;
        }

        const auto executableWriteTime = std::filesystem::last_write_time(outputPath, errorCode);
        if (errorCode) {
            return true;
        }

        for (const auto& objectFile : objectFiles) {
            const std::filesystem::path objectPath(objectFile);
            if (!std::filesystem::exists(objectPath, errorCode)) {
                return true;
            }

            const auto objectWriteTime = std::filesystem::last_write_time(objectPath, errorCode);
            if (errorCode || objectWriteTime > executableWriteTime) {
                return true;
            }
        }

        return false;
    }

    std::string BuildCompileCommand(const ProgramCompilerContext& context,
                                    const std::string& clangPath,
                                    const std::string& sdkPath,
                                    const std::string& sourceFile,
                                    const SourceBuildPaths& paths) {
        std::ostringstream command;
        command << QuoteForShell(clangPath);
        command << " -c " << QuoteForShell(sourceFile);
        command << " -o " << QuoteForShell(paths.objectPath.generic_string());
        command << " -MMD -MF " << QuoteForShell(paths.dependencyPath.generic_string());
        command << " -MT " << QuoteForShell(paths.objectPath.generic_string());
        AppendPrefixedPaths(command, context.includePaths, "-I");
        AppendPrefixedPaths(command, context.defines, "-D");
        command << " -isysroot " << QuoteForShell(sdkPath);
        command << " -std=c++23 -Wno-return-type-c-linkage -O0 -g";
        command << " -arch arm64";
        command << " -mmacosx-version-min=13.0";
        command << " -nostdinc++ -isystem " << QuoteForShell(std::string(ClangRoot) + "/include/c++/v1");
        return command.str();
    }

    std::string BuildLinkCommand(const ProgramCompilerContext& context,
                                 const std::string& clangPath,
                                 const std::string& sdkPath,
                                 const std::vector<std::string>& objectFiles,
                                 const std::string& outputPath) {
        std::ostringstream command;
        command << QuoteForShell(clangPath);

        for (const auto& objectFile : objectFiles) {
            command << " " << QuoteForShell(objectFile);
        }

        AppendPrefixedPaths(command, context.libraryPaths, "-L");

        for (const auto& library : context.libraries) {
            command << " -l" << library;
        }

        for (const auto& framework : context.frameworks) {
            command << " -framework " << framework;
        }

        command << " -isysroot " << QuoteForShell(sdkPath);
        command << " -arch arm64";
        command << " -mmacosx-version-min=13.0";
        command << " -L" << QuoteForShell(std::string(ClangRoot) + "/lib");
        command << " -Xlinker -rpath -Xlinker " << QuoteForShell(std::string(ClangRoot) + "/lib");
        command << " -lc++ -lc++abi";
        command << " -o " << QuoteForShell(outputPath);
        return command.str();
    }

    std::string JoinCommands(const std::vector<std::string>& commands) {
        if (commands.empty()) {
            return "Build outputs are up to date.";
        }

        std::ostringstream stream;
        for (size_t i = 0; i < commands.size(); ++i) {
            if (i > 0) {
                stream << "\n\n";
            }
            stream << commands[i];
        }
        return stream.str();
    }
}

ProgramCompiler::ProgramCompiler(std::string clangPath) : clangPath(std::move(clangPath)) {
}

ProgramCompilerResult ProgramCompiler::Compile(const ProgramCompilerContext& context) const {
    std::filesystem::path outputPath(context.outputPath);
    if (outputPath.has_parent_path()) {
        std::filesystem::create_directories(outputPath.parent_path());
    }

    CommandRunner commandRunner;
    ProgramCompilerResult result;
    const auto& sdkPath = ResolveSdkPath();
    if (sdkPath.empty()) {
        result.summary = "Compilation failed";
        result.output = "Failed to resolve the macOS SDK path using xcrun.";
        return result;
    }

    std::vector<std::string> commands;
    std::vector<std::string> objectFiles;
    objectFiles.reserve(context.sourceFiles.size());
    if (context.sourceFiles.empty()) {
        result.summary = "Compilation failed";
        result.output = "No source files were provided to the program compiler.";
        return result;
    }

    size_t compiledSourceCount = 0;
    std::string compilerOutput;

    for (const auto& sourceFile : context.sourceFiles) {
        const auto paths = CreateSourceBuildPaths(outputPath, sourceFile);
        if (paths.objectPath.has_parent_path()) {
            std::filesystem::create_directories(paths.objectPath.parent_path());
        }

        const auto compileSignature = BuildCompileSignature(context, clangPath, sdkPath, sourceFile);
        objectFiles.push_back(paths.objectPath.generic_string());

        if (!NeedsRecompile(sourceFile, paths, compileSignature)) {
            continue;
        }

        const auto compileCommand = BuildCompileCommand(context, clangPath, sdkPath, sourceFile, paths);
        commands.push_back(compileCommand);
        const auto compileResult = commandRunner.Run(compileCommand);
        compilerOutput += compileResult.result;

        if (compileResult.returnCode != 0) {
            std::error_code errorCode;
            std::filesystem::remove(paths.objectPath, errorCode);
            std::filesystem::remove(paths.dependencyPath, errorCode);
            std::filesystem::remove(paths.signaturePath, errorCode);

            result.command = JoinCommands(commands);
            result.summary = "Compilation failed";
            result.output = compilerOutput;
            return result;
        }

        if (!WriteTextFile(paths.signaturePath, compileSignature)) {
            result.command = JoinCommands(commands);
            result.summary = "Compilation failed";
            result.output = compilerOutput + "Failed to update compiler cache metadata.\n";
            return result;
        }

        ++compiledSourceCount;
    }

    const auto linkSignaturePath = outputPath.parent_path() / (outputPath.filename().string() + ".link.sig");
    const auto linkSignature = BuildLinkSignature(context, clangPath, sdkPath, objectFiles);
    const bool shouldLink = NeedsLink(outputPath, linkSignaturePath, linkSignature, objectFiles);
    if (shouldLink) {
        const auto linkCommand = BuildLinkCommand(context, clangPath, sdkPath, objectFiles, context.outputPath);
        commands.push_back(linkCommand);
        const auto linkResult = commandRunner.Run(linkCommand);
        compilerOutput += linkResult.result;

        if (linkResult.returnCode != 0) {
            std::error_code errorCode;
            std::filesystem::remove(linkSignaturePath, errorCode);

            result.command = JoinCommands(commands);
            result.summary = "Compilation failed";
            result.output = compilerOutput;
            return result;
        }

        if (!WriteTextFile(linkSignaturePath, linkSignature)) {
            result.command = JoinCommands(commands);
            result.summary = "Compilation failed";
            result.output = compilerOutput + "Failed to update linker cache metadata.\n";
            return result;
        }
    }

    const auto reusedSourceCount = context.sourceFiles.size() - compiledSourceCount;
    result.command = JoinCommands(commands);
    result.succeeded = true;
    if (compiledSourceCount == 0 && !shouldLink) {
        result.summary = "Compilation succeeded (up to date)";
        result.output = "Build outputs are up to date.\n";
        return result;
    }

    std::ostringstream summary;
    summary << "Compilation succeeded (compiled " << compiledSourceCount << ", reused " << reusedSourceCount;
    summary << (shouldLink ? ", linked)" : ", link reused)");
    result.summary = summary.str();
    result.output = compilerOutput;
    return result;
}
