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
// Created by sergei on 07/05/16.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../launcher/TestContext.hpp"
#include "json.hpp"

#include "intent/intent_service/IntentMatcher.hpp"
#include "intent/utils/Deserializer.hpp"
#include "intent/intent_service/EntitiesMatcher.hpp"
#include "intent/intent_service/IntentEncoder.hpp"

using namespace ::testing;

namespace intent
{
    namespace test
    {
        class IntentMatcherTest : public ::testing::Test
        {
        public:
            void SetUp()
            {
                const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

                std::string jsonContent = resourceManager.getResource(test::ResourceManager::ResourceId::INTENT_DICTIONARY_DESERIALIZATION_JSON_EXAMPLE);

                nlohmann::json json = nlohmann::json::parse(jsonContent);

                Deserializer deserializer;
                DictionaryModel dictionary;
                IntentModel intentModel;
                IntentServiceModel intentServiceModel = deserializer.deserialize<IntentServiceModel>(json);

                m_intentModel.reset(new IntentModel(*intentServiceModel.intentModel));
                m_dictionaryModel.reset(new DictionaryModel(*intentServiceModel.dictionaryModel));
            }

            std::unique_ptr<IntentModel> m_intentModel;
            std::unique_ptr<DictionaryModel> m_dictionaryModel;
        };

        const int BEVERAGE_ENTITY_ID = 0;
        const int NUMBER_ENTITY_ID = 1;
        const int PHONE_NUMBER_ENTITY_ID = 2;

        TEST_F(IntentMatcherTest, test_that_variables_are_named_by_default)
        {
            EntitiesMatcher::Variables variables;

            EntitiesMatcher::Variable number0;
            number0.entity = NUMBER_ENTITY_ID;
            number0.term = -1;
            number0.text = "un";
            variables.push_back(number0);

            EntitiesMatcher::Variable number1;
            number1.entity = NUMBER_ENTITY_ID;
            number1.term = -1;
            number1.text = "un";
            variables.push_back(number1);

            EntitiesMatcher::Variable beverage;
            beverage.entity = BEVERAGE_ENTITY_ID;
            beverage.term = -1;
            beverage.text = "coca";
            variables.push_back(beverage);

            const EntitiesMatcher::Variables& const_variables = const_cast<const EntitiesMatcher::Variables&>(variables);
            const IntentModel::IntentIndex intentModel = m_intentModel->intentsByIntentId;
            const IntentMatcher::IntentResult intentResult = IntentMatcher::match(*m_dictionaryModel, const_variables, intentModel);

            ASSERT_TRUE(intentResult.found);

            ASSERT_EQ("order3", intentResult.intent.intentId);

            ASSERT_EQ("@number0", intentResult.intent.entityMatches[0].name);
            ASSERT_EQ("@number1", intentResult.intent.entityMatches[1].name);
            ASSERT_EQ("@beverage0", intentResult.intent.entityMatches[2].name);
        }

        TEST_F(IntentMatcherTest, test_that_variables_are_named)
        {
            EntitiesMatcher::Variables variables;

            EntitiesMatcher::Variable softnumber;
            softnumber.entity = NUMBER_ENTITY_ID;
            softnumber.term = -1;
            softnumber.text = "un";
            variables.push_back(softnumber);

            EntitiesMatcher::Variable soft;
            soft.entity = BEVERAGE_ENTITY_ID;
            soft.term = -1;
            soft.text = "coca";
            variables.push_back(soft);

            EntitiesMatcher::Variable alcoholnumber;
            alcoholnumber.entity = NUMBER_ENTITY_ID;
            alcoholnumber.term = -1;
            alcoholnumber.text = "une";
            variables.push_back(alcoholnumber);

            EntitiesMatcher::Variable alcohol;
            alcohol.entity = BEVERAGE_ENTITY_ID;
            alcohol.term = -1;
            alcohol.text = "biere";
            variables.push_back(alcohol);

            const EntitiesMatcher::Variables& const_variables = const_cast<const EntitiesMatcher::Variables&>(variables);
            const IntentModel::IntentIndex intentModel = m_intentModel->intentsByIntentId;
            const IntentMatcher::IntentResult intentResult = IntentMatcher::match(*m_dictionaryModel, const_variables, intentModel);

            ASSERT_TRUE(intentResult.found);

            ASSERT_EQ("order2", intentResult.intent.intentId);

            ASSERT_EQ("softnumber", intentResult.intent.entityMatches[0].name);
            ASSERT_EQ("soft", intentResult.intent.entityMatches[1].name);
            ASSERT_EQ("alcoholnumber", intentResult.intent.entityMatches[2].name);
            ASSERT_EQ("alcohol", intentResult.intent.entityMatches[3].name);
        }

        TEST_F(IntentMatcherTest, test_that_regex_intent_is_matched)
        {
            EntitiesMatcher::Variables variables;

            EntitiesMatcher::Variable softnumber;
            softnumber.entity = NUMBER_ENTITY_ID;
            softnumber.term = -1;
            softnumber.text = "un";
            variables.push_back(softnumber);

            EntitiesMatcher::Variable soft;
            soft.entity = BEVERAGE_ENTITY_ID;
            soft.term = -1;
            soft.text = "coca";
            variables.push_back(soft);

            EntitiesMatcher::Variable alcoholnumber;
            alcoholnumber.entity = NUMBER_ENTITY_ID;
            alcoholnumber.term = -1;
            alcoholnumber.text = "une";
            variables.push_back(alcoholnumber);

            EntitiesMatcher::Variable alcohol;
            alcohol.entity = BEVERAGE_ENTITY_ID;
            alcohol.term = -1;
            alcohol.text = "biere";
            variables.push_back(alcohol);
            variables.push_back(alcoholnumber);
            variables.push_back(alcohol);

            const EntitiesMatcher::Variables& const_variables = const_cast<const EntitiesMatcher::Variables&>(variables);
            IntentModel::IntentIndex intentModel = m_intentModel->intentsByIntentId;

            IntentModel::Intent intent;
            intent.example = "regex_example";
            intent.entities = {0xFFFD, 1, 0, 0xFFFC, 0xFFFA};
            intentModel.insert(std::make_pair(IntentEncoder::encode({0xFFFD, 1, 0, 0xFFFC, 0xFFFA}), intent));

            const IntentMatcher::IntentResult intentResult = IntentMatcher::match(*m_dictionaryModel, const_variables, intentModel);

            ASSERT_TRUE(intentResult.found);
            ASSERT_EQ(IntentEncoder::encode({0xFFFD, 1, 0, 0xFFFC, 0xFFFA}), intentResult.intent.intentId);
        }

    }
}
