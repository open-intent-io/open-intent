#include <gtest/gtest.h>

#include "TestContext.hpp"

#include "intent/utils/Logger.hpp"

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // Adding environment that loads resource files
    const ::testing::Environment * env = ::testing::AddGlobalTestEnvironment(new intent::test::TestContext(RESSOURCES_DIRECTORY));
    intent::test::gTestContext = static_cast<const intent::test::TestContext *>(env);
    return RUN_ALL_TESTS();
}

