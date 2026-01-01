//
// Created by Jeppe Nielsen on 13/01/2024.
//

#include "ModuleSettings.hpp"

ModuleCompiler ModuleSettings::CreateCompiler() {
    return ModuleCompiler("/Users/jeppe/Jeppes/Scripting/clang/bin/clang++");
}
