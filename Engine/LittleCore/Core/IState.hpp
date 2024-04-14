//
// Created by Jeppe Nielsen on 12/04/2024.
//
#pragma once

namespace LittleCore {
    struct IState {
        virtual ~IState() {}
        virtual void Initialize() = 0;
        virtual void Update(float dt) = 0;
        virtual void Render() = 0;
        virtual void HandleEvent(void* event);
        void* mainWindow;
    };
}
