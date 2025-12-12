//
// Created by Jeppe Nielsen on 02/12/2025.
//

#include "MaxRectsPacker.hpp"

using namespace LittleCore;

MaxRectsPacker::MaxRectsPacker(uint16_t width, uint16_t height)
        : binWidth(width), binHeight(height) {}

void MaxRectsPacker::reset(uint16_t width, uint16_t height) {
    binWidth = width;
    binHeight = height;
    used.clear();
    free.clear();
    free.push_back({0, 0, width, height});
}

uint16_t MaxRectsPacker::width() const { return binWidth; }

uint16_t MaxRectsPacker::height() const { return binHeight; }

bool MaxRectsPacker::insert(uint16_t width, uint16_t height, Rect& out) {
    int bestArea = INT32_MAX;
    int bestShortSide = INT32_MAX;
    int bestIndex = -1;
    Rect bestRect;

    for (int i = 0; i < (int) free.size(); ++i) {
        const Rect& r = free[i];
        if (width <= r.width && height <= r.height) {
            int areaFit = int(r.width - width) * int(r.height - height);
            int shortSide = std::min<int>(r.width - width, r.height - height);
            if (areaFit < bestArea || (areaFit == bestArea && shortSide < bestShortSide)) {
                bestRect = {r.x, r.y, width, height};
                bestArea = areaFit;
                bestShortSide = shortSide;
                bestIndex = i;
            }
        }
    }

    if (bestIndex == -1)
        return false;

    place(bestRect);
    out = bestRect;
    return true;
}

const std::vector<MaxRectsPacker::Rect>& MaxRectsPacker::usedRects() const { return used; }

void MaxRectsPacker::place(const Rect& rect) {
    for (int i = 0; i < (int) free.size();) {
        if (splitFree(free[i], rect)) {
            free.erase(free.begin() + i);
        } else {
            ++i;
        }
    }

    pruneFree();
    used.push_back(rect);
}

bool MaxRectsPacker::intersects(const Rect& a, const Rect& b) {
    return !(b.x >= a.x + a.width ||
             b.x + b.width <= a.x ||
             b.y >= a.y + a.height ||
             b.y + b.height <= a.y);
}

bool MaxRectsPacker::splitFree(const Rect& freeRect, const Rect& usedRect) {
    if (!intersects(freeRect, usedRect))
        return false;

    if (usedRect.x > freeRect.x) {
        free.push_back({
                               freeRect.x, freeRect.y,
                               uint16_t(usedRect.x - freeRect.x), freeRect.height
                       });
    }

    if (usedRect.x + usedRect.width < freeRect.x + freeRect.width) {
        free.push_back({
                               uint16_t(usedRect.x + usedRect.width), freeRect.y,
                               uint16_t((freeRect.x + freeRect.width) - (usedRect.x + usedRect.width)),
                               freeRect.height
                       });
    }

    if (usedRect.y > freeRect.y) {
        free.push_back({
                               freeRect.x, freeRect.y,
                               freeRect.width, uint16_t(usedRect.y - freeRect.y)
                       });
    }

    if (usedRect.y + usedRect.height < freeRect.y + freeRect.height) {
        free.push_back({
                               freeRect.x, uint16_t(usedRect.y + usedRect.height),
                               freeRect.width,
                               uint16_t((freeRect.y + freeRect.height) - (usedRect.y + usedRect.height))
                       });
    }

    return true;
}

void MaxRectsPacker::pruneFree() {
    for (int i = 0; i < (int) free.size(); ++i) {
        for (int j = i + 1; j < (int) free.size(); ++j) {
            if (contains(free[i], free[j])) {
                free.erase(free.begin() + j);
                --j;
            } else if (contains(free[j], free[i])) {
                free.erase(free.begin() + i);
                --i;
                break;
            }
        }
    }
}

bool MaxRectsPacker::contains(const Rect& a, const Rect& b) {
    return b.x >= a.x &&
           b.y >= a.y &&
           b.x + b.width <= a.x + a.width &&
           b.y + b.height <= a.y + a.height;
}
