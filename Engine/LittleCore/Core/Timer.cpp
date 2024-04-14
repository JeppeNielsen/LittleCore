//
// Created by Jeppe Nielsen on 13/04/2024.
//

#include "Timer.hpp"

using namespace LittleCore;

void Timer::Start() {
    startTime = Time::now();
}

float Timer::Stop() {
    std::chrono::duration<float> elapsed = Time::now() - startTime;
    return elapsed.count();
}

