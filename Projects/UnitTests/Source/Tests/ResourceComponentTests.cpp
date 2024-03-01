//
// Created by Jeppe Nielsen on 29/02/2024.
//
#include "gtest/gtest.h"
#include "ResourceComponent.hpp"

using namespace LittleCore;

namespace {

    struct Mesh : ResourceComponent<Mesh> {
        int vertices = 0;
    };

    TEST(ResourceComponent, PtrOperatorShouldReturnBaseValuesOnInvalidResourceHandle) {

        Mesh meshComponent;
        meshComponent->vertices = 23;

        EXPECT_EQ(meshComponent->vertices, 23);
    }

}