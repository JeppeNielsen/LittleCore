//
// Created by Jeppe Nielsen on 20/03/2026.
//

#include "TargetProjectSettings.hpp"
#include "../Programs/ProgramCompilerContext.hpp"
#include "FileHelper.hpp"
#include <filesystem>
#include <regex>
#include <set>

namespace {
    std::string NormalizeDirectory(const std::filesystem::path& path) {
        return path.lexically_normal().generic_string();
    }

    void AddUnique(std::set<std::string>& values, const std::filesystem::path& path) {
        values.insert(NormalizeDirectory(path));
    }
}

bool TargetProjectSettings::TryParseStateTypeName(const std::string& source, std::string& stateTypeName) {
    static const std::regex stateRegex(
            R"((?:struct|class)\s+([A-Za-z_][A-Za-z0-9_]*)\s*:\s*[^{;]*\b(?:IState|MainState)\b)",
            std::regex::optimize);

    std::smatch match;
    if (!std::regex_search(source, match, stateRegex) || match.size() < 2) {
        return false;
    }

    stateTypeName = match[1].str();
    return true;
}

void TargetProjectSettings::Populate(ProgramCompilerContext& sharedContext, std::vector<DiscoveredProgram>& programs) const {
    sharedContext = {};
    programs.clear();

    const std::filesystem::path workspace(workspaceRoot);
    const std::filesystem::path projectRoot(rootPath);

    std::set<std::string> includePaths;

    AddUnique(includePaths, projectRoot);
    AddUnique(includePaths, workspace / "External/imgui");
    AddUnique(includePaths, workspace / "External/imgui/misc/cpp");
    AddUnique(includePaths, workspace / "External/netImgui/Code/Client");
    AddUnique(includePaths, workspace / "External/entt/src");
    AddUnique(includePaths, workspace / "External/glm");
    AddUnique(includePaths, workspace / "External/glaze/include");
    AddUnique(includePaths, workspace / "External/stb");
    AddUnique(includePaths, workspace / "External/ImGuizmo");
    AddUnique(includePaths, workspace / "Engine/Application/EditorSimulations");
    AddUnique(includePaths, workspace / "Engine/Application/Gui");
    AddUnique(includePaths, workspace / "Engine/Application/Project");
    AddUnique(includePaths, workspace / "Engine/Application/State");
    AddUnique(includePaths, workspace / "Engine/Application/Utilities");
    AddUnique(includePaths, workspace / "Engine/Application/Windows");
    AddUnique(includePaths, workspace / "Engine/ImGui");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Core");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Fibers");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Files");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Logic/Coloring");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Logic/Fonts");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Logic/Hierarchy");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Logic/Input");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Logic/Labels");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Logic/Movement");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Logic/Picking");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Logic/Prefabs");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Logic/Registry");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Logic/Rendering");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Logic/Simulation");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Logic/Spatial");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Logic/Systems");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Logic/Transform");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Math");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Meta");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Reflection");
    AddUnique(includePaths, workspace / "Engine/LittleCore/ResourceLoaders");
    AddUnique(includePaths, workspace / "Engine/LittleCore/ResourceManagement");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Serialization");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Tuples");
    AddUnique(includePaths, workspace / "Engine/LittleCore/Utilities");
    AddUnique(includePaths, workspace / "Engine/Netimgui");
    AddUnique(includePaths, workspace / "External/sokol");
    AddUnique(includePaths, workspace / "Engine/Sokol");

    if (std::filesystem::exists(projectRoot)) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(projectRoot)) {
            if (!entry.is_regular_file()) {
                continue;
            }

            const auto filePath = entry.path();
            const auto extension = filePath.extension().string();
            if (extension != ".cpp" && extension != ".hpp") {
                continue;
            }

            AddUnique(includePaths, filePath.parent_path());
            const auto normalizedPath = NormalizeDirectory(filePath);

            if (extension == ".hpp") {
                continue;
            }

            std::string stateTypeName;
            const auto source = LittleCore::FileHelper::ReadAllText(normalizedPath);
            if (TryParseStateTypeName(source, stateTypeName)) {
                programs.push_back({
                        filePath.stem().string(),
                        stateTypeName,
                        normalizedPath
                });
            } else {
                sharedContext.sourceFiles.push_back(normalizedPath);
            }
        }
    }

    sharedContext.includePaths.assign(includePaths.begin(), includePaths.end());
    sharedContext.libraryPaths.push_back((workspace / "bin/Debug").generic_string());
    sharedContext.libraries = {
            "sokol",
            "LittleCore",
            "ImGui",
            "ShaderCompiler",
            "Netimgui",
            "Application"
    };
    sharedContext.frameworks = {
            "QuartzCore",
            "Cocoa",
            "IOKit",
            "Carbon",
            "CoreAudio",
            "AudioToolbox",
            "Metal",
            "MetalKit",
            "OpenGL"
    };
    sharedContext.defines = {
            "XWIN_COCOA=1",
            "NDEBUG"
    };
}

bool TargetProjectSettings::HasValidRoot() const {
    return !rootPath.empty() && std::filesystem::exists(rootPath);
}

bool TargetProjectSettings::HasValidEngineAssetsPath() const {
    return !engineAssetsPath.empty() && std::filesystem::exists(engineAssetsPath);
}
