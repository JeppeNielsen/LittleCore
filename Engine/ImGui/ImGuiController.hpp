//
// Created by Jeppe Nielsen on 14/04/2024.
//


#pragma once
#include <functional>
#include <string>

namespace LittleCore {
    class ImGuiController {
    public:

        ~ImGuiController();

        using RenderFunction = std::function<void()>;

        void Initialize(void* mainWindow, RenderFunction renderFunction);
        void HandleEvent(void* event);
        void Render();
        void Destroy();
        void LoadFont(const std::string& fontPath, float fontSize);

    private:
        void* mainWindow;
        RenderFunction renderFunction;

    };
}
