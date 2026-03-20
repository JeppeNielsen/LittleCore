//
// Created by Jeppe Nielsen on 29/04/2024.
//

#pragma once
#include <string>
#include "CommandResult.hpp"

class CommandRunner {
public:
    CommandResult Run(std::string command) const;
};
