//
// Created by Jeppe Nielsen on 03/03/2024.
//

#include <gtest/gtest.h>
#include "ImageLoader.hpp"

using namespace LittleCore;

namespace {

    TEST(ImageLoaderTests, TestLoadFromFile) {
        int sizeX;
        int sizeY;

        ImageLoader::TryLoadImage("/Users/jeppe/Jeppes/LittleCore/Projects/UnitTests/Assets/Circle.psd", [&](unsigned char* data, int width, int height) {
            sizeX = width;
            sizeY = height;
        });

        EXPECT_EQ(256, sizeX);
        EXPECT_EQ(256, sizeY);
    }

}