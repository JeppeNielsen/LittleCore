//
// Created by Jeppe Nielsen on 18/01/2025.
//

#include "PathHelper.hpp"
#include <cstdlib>
#include <string>
#include <iostream>

void PathHelper::RevealPath(const std::string& path) {
    std::string command = "open -R \"" + path + "\"";

    // Execute the command
    int result = std::system(command.c_str());

    if (result != 0) {
        std::cerr << "Failed to reveal file in Finder. Command exited with code: " << result << std::endl;
    }
}
