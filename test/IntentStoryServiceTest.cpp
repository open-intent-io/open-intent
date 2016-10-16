/*
|---------------------------------------------------------|
|    ___                   ___       _             _      |
|   / _ \ _ __   ___ _ __ |_ _|_ __ | |_ ___ _ __ | |_    |
|  | | | | '_ \ / _ \ '_ \ | || '_ \| __/ _ \ '_ \| __|   |
|  | |_| | |_) |  __/ | | || || | | | ||  __/ | | | |_    |
|   \___/| .__/ \___|_| |_|___|_| |_|\__\___|_| |_|\__|   |
|        |_|                                              |
|                                                         |
|     - The users first...                                |
|                                                         |
|     Authors:                                            |
|        - Clement Michaud                                |
|        - Sergei Kireev                                  |
|                                                         |
|     Version: 1.0.0                                      |
|                                                         |
|---------------------------------------------------------|

The MIT License (MIT)
Copyright (c) 2016 - Clement Michaud, Sergei Kireev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
//
// Created by clement on 12/05/16.
//

#include <gtest/gtest.h>

#include "launcher/ResourceManager.hpp"
#include "launcher/TestContext.hpp"
#include "json.hpp"

#include "intent/intent_story_service/IntentStoryService.hpp"
#include "intent/intent_service/IntentEncoder.hpp"

#include "intent/utils/Deserializer.hpp"

namespace intent
{
    namespace test
    {
        class IntentStoryServiceBeverageTest : public ::testing::Test
        {
        public:
            void SetUp()
            {
                const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

                std::string jsonContent = resourceManager.getResource(
                        test::ResourceManager::ResourceId::INTENT_DICTIONARY_DESERIALIZATION_JSON_EXAMPLE);

                nlohmann::json json = nlohmann::json::parse(jsonContent);

                Deserializer deserializer;
                m_intentStoryServiceModel = deserializer.deserialize<IntentStoryServiceModel>(json);
            }
            const int BEVERAGE_ENTITY_ID = 0;
            const int NUMBER_ENTITY_ID = 1;

            IntentStoryServiceModel m_intentStoryServiceModel;
        };

        TEST_F(IntentStoryServiceBeverageTest, simple_matching)
        {
            IntentStoryService intentStoryService(m_intentStoryServiceModel);

            IntentStoryService::Result expectedResult;
            expectedResult.intent.intentId = "order1";

            expectedResult.found = true;
            IntentStoryService::EntityMatch m1, m2;
            m1.entity = "@number";
            m1.term = "2";
            m1.text = "2";
            m2.entity = "@beverage";
            m2.term = "Kronenbourg";
            m2.text = "Kro";
            expectedResult.intent.entityMatches.push_back(m1);
            expectedResult.intent.entityMatches.push_back(m2);
            expectedResult.nextStateId = "credit_card";

            EXPECT_EQ(expectedResult, intentStoryService.evaluate("root", "Bonjour, je voudrais 2 Kro"));
        }

        TEST_F(IntentStoryServiceBeverageTest, matching_order2)
        {
            IntentStoryService intentStoryService(m_intentStoryServiceModel);

            IntentStoryService::Result expectedResult;

            expectedResult.found = true;
            expectedResult.intent.intentId = "order2";
            IntentStoryService::EntityMatch m1, m2, m3, m4;
            m1.entity = "@number";
            m1.term = "2";
            m1.text = "2";

            m2.entity = "@beverage";
            m2.term = "Kronenbourg";
            m2.text = "Kro";

            m3.entity = "@number";
            m3.term = "4";
            m3.text = "quatre";

            m4.entity = "@beverage";
            m4.term = "Heineken";
            m4.text = "Heineken";

            expectedResult.intent.entityMatches.push_back(m1);
            expectedResult.intent.entityMatches.push_back(m2);
            expectedResult.intent.entityMatches.push_back(m3);
            expectedResult.intent.entityMatches.push_back(m4);

            expectedResult.nextStateId = "pay_2_conso";

            EXPECT_EQ(expectedResult, intentStoryService.evaluate("root", "Bonjour, je voudrais 2 Kro et quatre Heineken"));
        }

        TEST_F(IntentStoryServiceBeverageTest, not_matching)
        {
            IntentStoryService intentStoryService(m_intentStoryServiceModel);

            EXPECT_FALSE(intentStoryService.evaluate("root", "Bonjour, je voudrais de la Kro").found);
        }
    }


    class OrderBeverageStoryTest : public ::testing::Test
    {
    public:
        void SetUp()
        {
            const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

            std::string jsonContent = resourceManager.getResource(
                    test::ResourceManager::ResourceId::ORDER_BEVERAGE_INTENT_JSON);

            nlohmann::json json = nlohmann::json::parse(jsonContent);

            Deserializer deserializer;
            m_intentStoryServiceModel = deserializer.deserialize<IntentStoryServiceModel>(json);
        }

        IntentStoryServiceModel m_intentStoryServiceModel;
    };

    TEST_F(OrderBeverageStoryTest, order_without_appending)
    {
        IntentStoryService intentStoryService(m_intentStoryServiceModel);

        IntentStoryService::Result askWaiterResult = intentStoryService.evaluate("init", "Bob!");

        ASSERT_TRUE(askWaiterResult.found);
        EXPECT_EQ("ask_waiter", askWaiterResult.intent.intentId);

        IntentStoryService::Result orderOneResult = intentStoryService.evaluate(
                "wait_order", "Je veux trois Kronenbourg et 2 Coca");

        ASSERT_TRUE(orderOneResult.found);
        EXPECT_EQ("order2", orderOneResult.intent.intentId);

        IntentStoryService::Result appendAnswer = intentStoryService.evaluate(
                "wait_another_order", "Rien");

        ASSERT_TRUE(appendAnswer.found);
        EXPECT_EQ("nothing", appendAnswer.intent.intentId);
    }
}
