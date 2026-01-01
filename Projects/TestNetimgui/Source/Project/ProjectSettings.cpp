//
// Created by Jeppe Nielsen on 24/11/2024.
//

#include "ProjectSettings.hpp"
#include "FileHelper.hpp"
#include <iostream>

using namespace LittleCore;

bool IsPathModuleFile(const std::string& path) {
    auto allText = FileHelper::ReadAllText(path);
    auto pos = allText.find("CreateModule");
    return pos!=std::string::npos;
}

void HandleCppFile(const std::string& path, ModuleCompilerContext &moduleCompilerContext,
                   ModuleDefinitionsManager &moduleDefinitionsManager) {

    if (!IsPathModuleFile(path)) {
        moduleCompilerContext.cppFiles.push_back(path);
    } else {
        std::string id = FileHelper::GetFilenameWithoutExtension(path);
        moduleDefinitionsManager.CreateFromMainFile(id, path);
    }
}

void ProjectSettings::ModifyModules(ModuleCompilerContext &moduleCompilerContext,
                                    ModuleDefinitionsManager &moduleDefinitionsManager) {
    FileHelper::IterateFilesRecursively(rootPath, [&](std::string path) {
        std::string extension = FileHelper::GetExtension(path);
        if (extension == ".cpp") {
            HandleCppFile(path, moduleCompilerContext, moduleDefinitionsManager);

        } else if (extension == ".hpp") {
            moduleCompilerContext.hppFiles.push_back(path);
        }
        std::cout << path << ", extension: " << extension << std::endl;
    });
}
