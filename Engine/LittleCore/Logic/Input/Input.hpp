//
// Created by Jeppe Nielsen on 16/01/2024.
//

#pragma once
#include "Math.hpp"
#include "InputKey.hpp"
#include <vector>
#include <string>

namespace LittleCore {

    struct TouchPosition {
        vec2 position;
    };

    struct InputTouch {
        int index;
    };

    using TouchCollection = std::vector<InputTouch>;

    using KeyCollection = std::vector<InputKey>;

    struct Input {
        TouchPosition touchPosition[10];
        TouchCollection touchesDown;
        TouchCollection touchesUp;
        KeyCollection keysDown;
        KeyCollection keysUp;

        bool KeyIsInCollection(KeyCollection& keyCollection, InputKey key) {
            for(auto k : keyCollection) {
                if (k == key) {
                    return true;
                }
            }
            return false;
        }

        bool IsKeyDown(InputKey key) {
            return KeyIsInCollection(keysDown, key);
        }

        bool IsKeyUp(InputKey key) {
            return KeyIsInCollection(keysUp, key);
        }

    };
}

