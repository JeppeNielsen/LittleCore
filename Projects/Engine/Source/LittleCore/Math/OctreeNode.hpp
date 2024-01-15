//
//  OctreeNode.hpp
//  Tiny
//
//  Created by Jeppe Nielsen on 19/08/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once

namespace LittleCore {

    template<typename T>
    struct Octree;

    template<typename T>
    struct OctreeNode {
        BoundingBox box;
        Octree<T>* node;
        size_t childIndex;
        int order;
        T data;
    };


}
