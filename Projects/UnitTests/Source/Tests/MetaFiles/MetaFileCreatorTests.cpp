//
// Created by Jeppe Nielsen on 08/03/2024.
//

#include <gtest/gtest.h>

#include "MetaFileCreator.hpp"

using namespace LittleCore;

namespace {

    TEST(MetaFileCreator, GetMetaPath) {

        MetaFileCreator metaFileCreator;
        auto metaPath = metaFileCreator.GetMetaPath("Spaceship.png");

        EXPECT_EQ("Spaceship.png.meta", metaPath);

    }

    TEST(MetaFileCreator, CreateMetaFile) {

        MetaFileCreator metaFileCreator;

        std::string path = "../../../../Assets/Circle.psd";

        std::string guid;
        if (!metaFileCreator.TryGetMetaGuid(path, guid)) {
            guid = metaFileCreator.CreateMetaFile(path);
        }

        EXPECT_EQ(guid.size(), 32);

        std::string foundGuid;
        bool success = metaFileCreator.TryGetMetaGuid(path, foundGuid);

        EXPECT_TRUE(success);
        EXPECT_EQ(guid, foundGuid);
    }

    TEST(MetaFileCreator, TryGetMetaGuidForNonExistingPath) {
        MetaFileCreator metaFileCreator;
        std::string foundGuid;
        bool succes = metaFileCreator.TryGetMetaGuid("Weird path", foundGuid);

        EXPECT_FALSE(succes);
        EXPECT_EQ("", foundGuid);
    }
}
