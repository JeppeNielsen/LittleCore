//
//  BoundingBox.hpp
//  Tiny
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Math.hpp"

namespace LittleCore {
    class BoundingBox {
    public:
        BoundingBox();
        BoundingBox(const BoundingBox& other);
        BoundingBox(const glm::vec3& center, const glm::vec3& extends);
        ~BoundingBox();
        
        bool Intersects(const BoundingBox& other) const;
        bool Contains(const BoundingBox& other) const;
        float Volume() const;

        glm::vec3 center;
        glm::vec3 extends;

        bool operator==(const BoundingBox &other);
        bool operator!=(const BoundingBox &other);

        void CreateWorldAligned(const glm::mat4x4& matrix, BoundingBox& boundingBox) const;
    };
}

