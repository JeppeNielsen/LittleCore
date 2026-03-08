//
// Created by Jeppe Nielsen on 12/04/2024.
//
#pragma once
#include <memory>
#include "EngineSettings.hpp"
#include "IState.hpp"
#include <functional>

struct sapp_event;

namespace LittleCore {
    class Engine {
    public:
        Engine(EngineSettings settings);
        template<typename T>
        void Start(const std::function<void(T& state)>& onCreated = nullptr) {
            MainLoop([this, &onCreated] (){
                state = new T();
                if (onCreated) {
                    onCreated(*(T*)state);
                }
            },[this]() {
                delete state;
                state = nullptr;
            });
        }
    private:
        EngineSettings settings;
        IState* state = nullptr;
        std::function<void()> onInitializeCallback;
        std::function<void()> onDestroyCallback;
        bool graphicsInitialized = false;

        static void AppInit(void* userData);
        static void AppFrame(void* userData);
        static void AppCleanup(void* userData);
        static void AppEvent(const sapp_event* event, void* userData);

        void OnAppInit();
        void OnAppFrame();
        void OnAppCleanup();
        void OnAppEvent(const sapp_event* event);
        void MainLoop(const std::function<void()>& onInitialized,  const std::function<void()>& onDestroy);
    };
}
