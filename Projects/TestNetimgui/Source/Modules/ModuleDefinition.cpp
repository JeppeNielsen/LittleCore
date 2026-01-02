//
// Created by Jeppe Nielsen on 13/01/2024.
//

#include "ModuleDefinition.hpp"
#include "ModuleSettings.hpp"
#include <filesystem>

ModuleDefinition::ModuleDefinition(ModuleSettings &settings,const std::string &id, const std::string &mainPath) :
settings(settings), id(id), mainPath(mainPath), compiler(settings.CreateCompiler()) {

}

bool ModuleDefinition::LibraryExists() const {
    return std::__fs::filesystem::exists(LibraryPath());
}

std::string ModuleDefinition::LibraryPath() const {
    std::string libName = id + "";
    std::__fs::filesystem::path libPath = settings.libraryFolder;
    return libPath.concat(libName);
}

ModuleCompilerResult ModuleDefinition::Build() {
    ModuleCompilerContext context = settings.context;
    context.cppFiles.push_back(mainPath);
    context.outputPath = LibraryPath();

    auto result = compiler.Compile(context);
    return result;
}
