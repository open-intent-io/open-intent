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
// Created by clement on 07/05/16.
//

//
// Created by clement on 07/05/16.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "launcher/TestContext.hpp"
#include "json.hpp"

#include "intent/intent_service/IntentService.hpp"
#include "intent/utils/Deserializer.hpp"
#include "intent/intent_service/IntentEncoder.hpp"

using namespace ::testing;

namespace intent
{
    namespace test
    {
        class IntentServiceBeverageTest : public ::testing::Test
        {
        public:
            void SetUp()
            {
                const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

                std::string jsonContent = resourceManager.getResource(test::ResourceManager::ResourceId::INTENT_DICTIONARY_DESERIALIZATION_JSON_EXAMPLE);

                nlohmann::json json = nlohmann::json::parse(jsonContent);

                Deserializer deserializer;
                m_intentServiceModel = deserializer.deserialize<IntentServiceModel>(json);

            }

            IntentServiceModel m_intentServiceModel;
        };


        TEST_F(IntentServiceBeverageTest, test_if_simple_intent_is_correctly_found)
        {
            IntentService intentService(m_intentServiceModel);

            EXPECT_FALSE(intentService.evaluate("Coucou, je m'appelle John").found);

            IntentService::Result result;
            result.found = true;
            result.intent.intentId = "order1";
            IntentService::EntityMatch v;
            v.entity = "@number";
            v.term = "2";
            v.text = "2";

            result.intent.entityMatches.push_back(v);

            v.entity = "@beverage";
            v.term = "Kronenbourg";
            v.text = "Kro";

            result.intent.entityMatches.push_back(v);

            EXPECT_EQ(result, intentService.evaluate("Bonjour, j'aimerais 2 pintes de Kro"));
        }

        TEST_F(IntentServiceBeverageTest, test_order_one_coca_cola)
        {
            IntentService intentService(m_intentServiceModel);

            IntentService::Result result;
            result.found = true;
            result.intent.intentId = "order1";
            IntentService::EntityMatch v;
            v.entity = "@number";
            v.term = "2";
            v.text = "2";

            result.intent.entityMatches.push_back(v);

            v.entity = "@beverage";
            v.term = "Coca-Cola";
            v.text = "Coca-Cola";

            result.intent.entityMatches.push_back(v);

            EXPECT_EQ(result, intentService.evaluate("Bonjour, j'aimerais 2 Coca-Cola"));
        }



    }
}
