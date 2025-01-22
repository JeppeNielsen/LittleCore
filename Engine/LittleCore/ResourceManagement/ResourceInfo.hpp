//
// Created by Jeppe Nielsen on 21/01/2025.
//

#pragma once
#include <string>

namespace LittleCore {
    struct ResourceInfo {
        std::string id = "";
        std::string path = "";
        bool isMissing = false;
    };
}