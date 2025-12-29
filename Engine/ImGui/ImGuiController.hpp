//
// Created by Jeppe Nielsen on 14/04/2024.
//


#pragma once
#include <functional>
#include <string>
#include "imgui.h"

class ImFont;
class ImDrawData;

namespace LittleCore {
    class ImGuiController {
    public:

        ~ImGuiController();

        using RenderFunction = std::function<void()>;

        void Initialize(void* mainWindow, const RenderFunction& renderFunction);
        void HandleEvent(void* event);
        void Render();
        void Destroy();
        ImFont* LoadFont(const std::string& fontPath, float fontSize);

        void Draw(uint16_t viewId, ImDrawData* draw_data);

    private:
        void* mainWindow;
        RenderFunction renderFunction;
    };
}
