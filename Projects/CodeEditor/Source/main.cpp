//
// Created by Jeppe Nielsen on 30/03/2026.
//

#include "Engine.hpp"
#include "CodeEditor.hpp"
#include <filesystem>

namespace {
    struct LaunchOptions {
        std::string projectRoot;
        std::string engineRoot;
    };

    std::string ResolveWorkspaceRoot() {
        std::filesystem::path path(__FILE__);
        for (int i = 0; i < 4; ++i) {
            path = path.parent_path();
        }
        return path.lexically_normal().generic_string();
    }

    LaunchOptions ParseLaunchOptions(int argc, char** argv) {
        LaunchOptions options;

        for (int i = 1; i < argc; ++i) {
            const std::string argument = argv[i];
            if (argument == "--project-root" && i + 1 < argc) {
                options.projectRoot = argv[++i];
                continue;
            }

            if (argument == "--engine-root" && i + 1 < argc) {
                options.engineRoot = argv[++i];
                continue;
            }

            if (options.projectRoot.empty()) {
                options.projectRoot = argument;
            }
        }

        return options;
    }
}

struct Vector2 {
    float x;
    float y;
    
    float Length() { return x*y; }
    
    float GetNumber(int one, int two) {
        return one + two;
    }
    
    float Get(u_int32_t type)  {
        this->x += 2;
        auto bla = type + (uint32_t)x;
        Length()+2;
        return 321;
    }
    
    template<class T>
    float Run(const Vector2& other)  {
          return other.x;
    }

};

int main(int argc, char** argv) {
    using namespace LittleCore;

    const auto workspaceRoot = ResolveWorkspaceRoot();
    const auto options = ParseLaunchOptions(argc, argv);
    const auto projectRoot = options.projectRoot.empty()
                             ? workspaceRoot + "/Projects/CodeEditor"
                             : options.projectRoot;
    const auto engineRoot = options.engineRoot.empty()
                            ? workspaceRoot + "/Projects/TestNetimgui/Assets/"
                            : options.engineRoot;
    Engine engine({
        .mainWindowTitle = "CodeEditor",
        .showWindow = true
    });

    engine.Start<CodeEditor>([projectRoot, engineRoot](CodeEditor& state) {
        state.context.name = "CodeEditor";
        state.context.projectRoot = projectRoot;
        state.context.engineRoot = engineRoot;
    });
    
    Vector2 speed;
    
    speed.x += 23;
    speed.y = 12;
    
    speed.x += 23;
    
    speed.GetNumber(21,32);
    
    speed.Get(32);
    
    speed.Get(10);
    
    speed.Run<decltype(speed)>(speed);
    
    speed.x += speed.GetNumber(10,20);
    
    speed.y+=sinf(speed.y*10);
    
    speed.x+=speed.Get(10);
    
    speed.y += 10.0f;
    
    float test = speed.Run<int>(speed);
    
    auto path = ResolveWorkspaceRoot();
    
    speed.x += 2;
    
    
    return 0;
}


