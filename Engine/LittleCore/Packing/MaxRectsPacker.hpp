//
// Created by Jeppe Nielsen on 02/12/2025.
//
#pragma once
#pragma once
#include <vector>
#include <cstdint>
#include <algorithm>

namespace LittleCore {
    class MaxRectsPacker {
    public:
        struct Rect {
            uint16_t x = 0, y = 0, width = 0, height = 0;
        };

        MaxRectsPacker(uint16_t width = 0, uint16_t height = 0);

        void reset(uint16_t width, uint16_t height);

        uint16_t width() const;

        uint16_t height() const;

        bool insert(uint16_t width, uint16_t height, Rect& out);

        const std::vector<Rect>& usedRects() const;

    private:
        uint16_t binWidth = 0, binHeight = 0;
        std::vector<Rect> used;
        std::vector<Rect> free;

        void place(const Rect& rect);

        static bool intersects(const Rect& a, const Rect& b);

        bool splitFree(const Rect& freeRect, const Rect& usedRect);

        void pruneFree();

        static bool contains(const Rect& a, const Rect& b);
    };
}