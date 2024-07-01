//
// Created by Jeppe Nielsen on 29/04/2024.
//
#include <string>
#include <cstdlib>
#include <cstdio>
#include <array>
#include "CommandRunner.hpp"

CommandResult CommandRunner::Run(std::string command) {

    command.append(" 2>&1");

    std::array<char, 128> buffer;
    std::string result;

    //std::cout << "Opening reading pipe" << std::endl;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return {"Couldn't start command.", 0};
    }
    while (fgets(buffer.data(), 128, pipe) != NULL) {
        //std::cout << "Reading..." << std::endl;
        result += buffer.data();
    }
    int returnCode = pclose(pipe);
    return {result, returnCode};
}
