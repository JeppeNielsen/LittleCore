//
// Created by Jeppe Nielsen on 29/04/2024.
//

#include "CommandRunner.hpp"
#include <array>
#include <cstdio>
#include <sys/wait.h>

CommandResult CommandRunner::Run(std::string command) const {
    command.append(" 2>&1");

    std::array<char, 256> buffer{};
    std::string result;

    FILE* pipe = popen(command.c_str(), "r");
    if (pipe == nullptr) {
        return {"Couldn't start command.", -1};
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        result += buffer.data();
    }

    int returnCode = pclose(pipe);
    if (WIFEXITED(returnCode)) {
        returnCode = WEXITSTATUS(returnCode);
    }

    return {result, returnCode};
}
