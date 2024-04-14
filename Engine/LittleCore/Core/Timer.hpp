//
// Created by Jeppe Nielsen on 13/04/2024.
//
#pragma once
#include <chrono>

namespace LittleCore {
    class Timer {
    public:
        using Time = std::chrono::high_resolution_clock;
        void Start();
        float Stop();
    private:
        std::chrono::time_point<Time> startTime;
    };
}
