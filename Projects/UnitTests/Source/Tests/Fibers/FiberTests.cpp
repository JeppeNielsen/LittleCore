//
// Created by Jeppe Nielsen on 01/07/2024.
//

#include "gtest/gtest.h"
#include "Fiber.hpp"

using namespace LittleCore;

namespace {

    struct Counter {
        int counter = 0;
    };

    TEST(Fiber, StepTwice) {

        struct StepTwice : Counter {
            Fiber Test() {
                counter++;
                co_yield 0;
                counter++;
            }
        };

        StepTwice stepTwice;

        auto fiber = stepTwice.Test();
        fiber.Step();
        EXPECT_EQ(stepTwice.counter, 1);
        fiber.Step();
        EXPECT_EQ(stepTwice.counter, 2);
    }

    TEST(Fiber, NestedCalls) {

        struct NestedFunction : Counter {

            Fiber Test() {
                 counter++;
                 co_yield Child();
                 counter++;
            }

            Fiber Child() {
                counter++;
                co_yield 0;
                counter++;
            }

        };

        NestedFunction nestedFunction;
        auto fiber = nestedFunction.Test();
        int numberOfSteps = 0;
        while(fiber.Step()) { numberOfSteps++; }
        EXPECT_EQ(nestedFunction.counter, 4);
        EXPECT_EQ(numberOfSteps, 1);
    }

    TEST(Fiber, FiveYields) {

        struct FiveYields : Counter {
            Fiber Test() {
                for (int i = 0; i < 5; ++i) {
                    counter++;
                    co_yield 0;
                }
            }
        };

        FiveYields fiveYields;
        auto fiber = fiveYields.Test();
        int numberOfSteps = 0;
        while(fiber.Step()) { numberOfSteps++; }
        EXPECT_EQ(fiveYields.counter, 5);
        EXPECT_EQ(numberOfSteps, 5);
    }


    TEST(Fiber, StepOnceWithChildFunction) {

        struct Test : Counter {
            Fiber Animate() {
                counter++;
                co_yield ChildAnimation(0.5f);
                counter++;
            }

            Fiber ChildAnimation(float time) {
                counter++;
                co_yield 0;
            }
        };

        Test test;
        auto fiber = test.Animate();

        int numSteps = 0;
        while (fiber.Step()) {
            numSteps++;
        }

        EXPECT_EQ(test.counter, 3);
        EXPECT_EQ(numSteps, 1);
    }

    TEST(Fiber, OutParameter) {

        struct PowerOfTwoCalculator : Counter {

            Fiber CalculatePowerOfTwo(int count, int& result) {

                for (int i = 0; i < count; ++i) {
                    result *= 2;
                    co_yield 0;
                }
            }

        };

        int result = 1;
        PowerOfTwoCalculator calculator;
        auto fiber = calculator.CalculatePowerOfTwo(8, result);

        while (fiber.Step());

        EXPECT_EQ(result, 256);
    }

}