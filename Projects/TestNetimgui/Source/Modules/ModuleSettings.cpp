//
// Created by Jeppe Nielsen on 13/01/2024.
//

#include "ModuleSettings.hpp"

ModuleCompiler ModuleSettings::CreateCompiler() {

    //../../Scripting/clang18/lib
    ///Users/jeppe/Jeppes/Scripting/clang18
    ///Users/jeppe/Jeppes/Scripting/clang18/bin
    return ModuleCompiler("/Users/jeppe/Jeppes/Scripting/clang18/bin/clang++");
    //return ModuleCompiler("/Users/jeppe/Jeppes/Scripting/clang/bin/clang++");
}
