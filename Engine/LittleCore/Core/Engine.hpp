//
// Created by Jeppe Nielsen on 12/04/2024.
//
#pragma once
#include <memory>
#include "EngineSettings.hpp"
#include "IState.hpp"

namespace LittleCore {
    class Engine {
    public:
        Engine(EngineSettings settings);
        template<typename T>
        void Start() {
            state = std::make_unique<T>();
            MainLoop();
        }

    private:
        EngineSettings settings;
        std::unique_ptr<IState> state;
        void MainLoop();
    };
}