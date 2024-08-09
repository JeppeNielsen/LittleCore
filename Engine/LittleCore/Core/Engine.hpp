//
// Created by Jeppe Nielsen on 12/04/2024.
//
#pragma once
#include <memory>
#include "EngineSettings.hpp"
#include "IState.hpp"
#include <functional>

namespace LittleCore {
    class Engine {
    public:
        Engine(EngineSettings settings);
        template<typename T>
        void Start() {
            MainLoop([this] (){
                state = std::make_unique<T>();
            });
        }

    private:
        EngineSettings settings;
        std::unique_ptr<IState> state;
        void MainLoop(const std::function<void()>& gfxInitialized);
    };
}