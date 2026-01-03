//
// Created by Jeppe Nielsen on 13/01/2024.
//

#include "ModuleDefinition.hpp"
#include "ModuleSettings.hpp"
#include <filesystem>
#include "FileHelper.hpp"
#include "ProjectSettings.hpp"

ModuleDefinition::ModuleDefinition(ModuleSettings &settings, ProjectSettings& projectSettings, const std::string &id, const std::string &mainPath) :
settings(settings), projectSettings(projectSettings), id(id), mainPath(mainPath), compiler(settings.CreateCompiler()) {

}

bool ModuleDefinition::LibraryExists() const {
    return std::__fs::filesystem::exists(LibraryPath());
}

std::string ModuleDefinition::LibraryPath() const {
    std::string libName = id + "";
    std::__fs::filesystem::path libPath = settings.libraryFolder;
    return libPath.concat(libName);
}

#define SOURCE(...) #__VA_ARGS__

void Replace(std::string& s, const std::string& from, const std::string& to) {
    std::size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size(); // move past the replacement
    }
}

std::string ModuleDefinition::CreateMainContent() {
    std::string content = "";

    content += "#include \"Engine.hpp\"\n";
    //content += "#include \""+mainPath+"\"\n";

    content += SOURCE(
            int main() {
                using namespace LittleCore;
                Engine engine({
                                      .mainWindowTitle = "___NAME___",
                                      .showWindow = false
                              });
                engine.Start<___NAME___>([](___NAME___& state) {
                    state.context.name = "___NAME___";
                    state.context.projectRoot = "___PROJECT_ROOT___";
                    state.context.engineRoot = "___ENGINE_ROOT___";
                });
            }

    );

    Replace(content, "___NAME___", id);
    Replace(content, "___PROJECT_ROOT___", projectSettings.rootPath);
    Replace(content, "___ENGINE_ROOT___", projectSettings.engineRoot);

    return content;
}

std::string ModuleDefinition::EnsureMainFile() {
    std::string path = settings.libraryFolder + id + ".cpp";
    std::string maincontent = CreateMainContent();
    std::string mainStateContent = LittleCore::FileHelper::ReadAllText(mainPath);

    std::string all = mainStateContent + "\n\n" + maincontent;

    LittleCore::FileHelper::TryWriteAllText(path, all);

    return path;
}

bool ModuleDefinition::IsBuilding() const {
    return isBuilding;
}

ModuleCompilerResult ModuleDefinition::GetResult() const {
    return result;
}

void ModuleDefinition::StartBuild() {
    if (IsBuilding()) {
        return;
    }

    compilationTimer.Start();
    isBuilding = true;

    taskRunner.runAsyncTask([&, this]{
        ModuleCompilerContext context = settings.context;
        //context.cppFiles.push_back(mainPath);

        context.cppFiles.push_back(EnsureMainFile());

        context.outputPath = LibraryPath();

        auto result = compiler.Compile(context);

        for(auto error : result.errors) {
            errorsFromCompilation.push_back(error);
        }

    }, [&, this] {

        float duration = compilationTimer.Stop();
        std::cout << "Compilation took = " << std::to_string(duration) << "\n";
        isBuilding = false;

        errorsFromCompilation.push_back("Compilation took = " + std::to_string(duration) + "\n");

        result = {};
        result.errors = errorsFromCompilation;
        errorsFromCompilation.clear();

        for(auto& e : result.errors) {
            std::cout << e << "\n";
        }


    });

}

void ModuleDefinition::Update() {
    taskRunner.processTasks();
}
