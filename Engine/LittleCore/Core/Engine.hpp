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
                state = new T();
            },[this]() {
                delete state;
            });
        }
    private:
        EngineSettings settings;
        IState* state = nullptr;
        void MainLoop(const std::function<void()>& onInitialized,  const std::function<void()>& onDestroy);
    };
}