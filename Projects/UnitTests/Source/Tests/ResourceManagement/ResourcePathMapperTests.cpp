//
// Created by Jeppe Nielsen on 11/03/2024.
//
#include <gtest/gtest.h>
#include "ResourcePathMapper.hpp"


using namespace LittleCore;

namespace {

    TEST(ResourcePathMapper, RefreshFromRootPath) {
        ResourcePathMapper resourcePathMapper;

        std::string rootPath = "../../../../Assets/";

        resourcePathMapper.RefreshFromRootPath(rootPath);

        std::string expectedGuid = "B62D424BF40F46359248CDE498930422";
        std::string expectedPath = "../../../../Assets/Circle.psd";

        EXPECT_EQ(resourcePathMapper.GetGuid(expectedPath), expectedGuid);
        EXPECT_EQ(resourcePathMapper.GetPath(expectedGuid), expectedPath);

        std::string expectedSubFolderGuid = "F4BEEC6D07BF4DEBA129269849153375";
        std::string expectedSubfolderPath = "../../../../Assets/Prefabs/Cube.json";

        EXPECT_EQ(resourcePathMapper.GetGuid(expectedSubfolderPath), expectedSubFolderGuid);
        EXPECT_EQ(resourcePathMapper.GetPath(expectedSubFolderGuid), expectedSubfolderPath);
    }

}
