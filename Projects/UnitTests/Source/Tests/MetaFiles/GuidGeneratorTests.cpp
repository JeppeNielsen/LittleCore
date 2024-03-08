//
// Created by Jeppe Nielsen on 08/03/2024.
//
#include <gtest/gtest.h>

#include "GuidGenerator.hpp"

using namespace LittleCore;

namespace {

    TEST(GuidGenerator, CreateNew) {
        GuidGenerator guidGenerator;
        std::string guid = guidGenerator.GenerateNew();
        EXPECT_EQ(guid.size(), 32);
    }

}