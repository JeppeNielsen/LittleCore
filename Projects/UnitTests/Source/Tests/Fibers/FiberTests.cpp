//
// Created by Jeppe Nielsen on 01/07/2024.
//

#include "gtest/gtest.h"
#include "Fiber.hpp"

using namespace LittleCore;

namespace {

    static int TestCounter = 0;

    Fiber Function() {
        TestCounter++;
        co_yield 0;
        TestCounter++;
    }

    Fiber ChildFunction() {
        TestCounter++;
        co_yield 0;
        TestCounter++;
    }

    Fiber NestedFunction() {
        TestCounter++;
        co_yield ChildFunction();
        TestCounter++;
    }

    Fiber FiveYields() {
        for (int i = 0; i < 5; ++i) {
            TestCounter++;
            co_yield 0;
        }
    }

    TEST(Fiber, StepTwice) {

        TestCounter = 0;

        auto fiber = Function();
        fiber.Step();
        EXPECT_EQ(TestCounter, 1);
        fiber.Step();
        EXPECT_EQ(TestCounter, 2);
    }

    TEST(Fiber, NestedCalls) {

        TestCounter = 0;

        auto fiber = NestedFunction();
        int numberOfSteps = 0;
        while(fiber.Step()) { numberOfSteps++; }
        EXPECT_EQ(TestCounter, 4);
        EXPECT_EQ(numberOfSteps, 1);
    }

    TEST(Fiber, FiveYields) {

        TestCounter = 0;
        auto fiber = FiveYields();
        int numberOfSteps = 0;
        while(fiber.Step()) { numberOfSteps++; }
        EXPECT_EQ(TestCounter, 5);
        EXPECT_EQ(numberOfSteps, 5);
    }
}