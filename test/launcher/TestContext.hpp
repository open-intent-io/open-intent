//
// Created by clement on 05/05/16.
//

#ifndef INTENT_TEST_TESTCONTEXT_HPP
#define INTENT_TEST_TESTCONTEXT_HPP

#include <gtest/gtest.h>

#include "ResourceManager.hpp"

namespace intent
{
    namespace test
    {
        class TestContext : public ::testing::Environment
        {
        public:
            TestContext(const std::string &resourceDirectory);
            void SetUp();
            void TearDown();

            inline const intent::test::ResourceManager &getResourceManager() const { return m_resourceManager; }

        private:
            void registerResources();

            intent::test::ResourceManager m_resourceManager;
        };

        extern const intent::test::TestContext *gTestContext;
    }
}

#endif
