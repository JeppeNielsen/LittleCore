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
        ivec2 screenSize;
        TouchPosition touchPosition[10];
        TouchCollection touchesDown;
        TouchCollection touchesUp;
        KeyCollection keysDown;
        KeyCollection keysUp;

        bool KeyIsInCollection(const KeyCollection& keyCollection, InputKey key) const {
            for(auto k : keyCollection) {
                if (k == key) {
                    return true;
                }
            }
            return false;
        }

        bool IsKeyDown(InputKey key) const {
            return KeyIsInCollection(keysDown, key);
        }

        bool IsKeyUp(InputKey key) const {
            return KeyIsInCollection(keysUp, key);
        }

        bool TouchIsInCollection(const TouchCollection& touchCollection, InputTouch touch) const {
            for(auto t : touchCollection) {
                if (t.index == touch.index) {
                    return true;
                }
            }
            return false;
        }

        bool IsTouchDown(InputTouch touch) const {
            return TouchIsInCollection(touchesDown, touch);
        }

        bool IsTouchUp(InputTouch touch) const {
            return TouchIsInCollection(touchesUp, touch);
        }

        void Clear() {
            touchesDown.clear();
            touchesUp.clear();
            keysDown.clear();
            keysUp.clear();
        }

    };
}

