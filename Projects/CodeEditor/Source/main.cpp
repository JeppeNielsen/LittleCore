//
// Created by Jeppe Nielsen on 30/03/2026.
//

#include "Engine.hpp"
#include "CodeEditor.hpp"
#include <filesystem>

namespace {
    std::string ResolveWorkspaceRoot() {
        std::filesystem::path path(__FILE__);
        for (int i = 0; i < 4; ++i) {
            path = path.parent_path();
        }
        return path.lexically_normal().generic_string();
    }
}

struct Vector2 {
    float x;
    float y;
    
    float Length() { return x*y; }
    
    float GetNumber(int one, int two) {
        return one + two;
    }

};

int main() {
    using namespace LittleCore;

    const auto workspaceRoot = ResolveWorkspaceRoot();
    Engine engine({
        .mainWindowTitle = "CodeEditor",
        .showWindow = true
    });

    engine.Start<CodeEditor>([&workspaceRoot](CodeEditor& state) {
        state.context.name = "CodeEditor";
        state.context.projectRoot = workspaceRoot + "/Projects/CodeEditor";
        state.context.engineRoot = workspaceRoot + "/Projects/TestNetimgui/Assets/";
    });
    
    return 0;
}

