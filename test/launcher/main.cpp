//
// Created by clement on 05/05/16.
//

#include <gtest/gtest.h>

#include "TestContext.hpp"

#include "intent/utils/Logger.hpp"


int main(int argc, char **argv)
{
    intent::log::Logger::initialize(intent::log::Logger::SeverityLevel::FATAL);

    ::testing::InitGoogleTest(&argc, argv);

    // Adding environment that loads resource files
    const ::testing::Environment * env = ::testing::AddGlobalTestEnvironment(new intent::test::TestContext("res/"));
    intent::test::gTestContext = static_cast<const intent::test::TestContext *>(env);
    return RUN_ALL_TESTS();
}

