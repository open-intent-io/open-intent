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
// Created by clement on 05/05/16.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../launcher/TestContext.hpp"

#include <string>

#include "intent/utils/Deserializer.hpp"

#include <sstream>

using namespace intent;
using namespace ::testing;

#define EXPECT_EQ_SIGNED(v1, v2) EXPECT_EQ(v1, static_cast<int>(v2))
#define ASSERT_EQ_SIGNED(v1, v2) ASSERT_EQ(v1, static_cast<int>(v2))

void test_simple_deserialization(const DictionaryModel &dictionaryModel, const IntentModel &intentModel)
{
    EXPECT_EQ_SIGNED(4, intentModel.intentsByIntentId.size());
    EXPECT_EQ_SIGNED(3, dictionaryModel.entitiesByEntityId.size());

    EXPECT_EQ_SIGNED(19, dictionaryModel.dictionary.size());
    EXPECT_EQ_SIGNED(91, dictionaryModel.dictionary.index_size());
    EXPECT_EQ_SIGNED(1, dictionaryModel.regexesByEntityId.size());

    EXPECT_THAT(dictionaryModel.entitiesByEntityId, UnorderedElementsAre(std::pair<int, std::string>(0, "@beverage"),
                                                                   std::pair<int, std::string>(1, "@number"),
                                                                   std::pair<int, std::string>(2, "@phonenumber")));

    Term t1;
    t1.term = "Coca-Cola";
    t1.lowerCaseTerm = "coca-cola";
    t1.entityId = 0;
    t1.termId = 0;
    t1.alias.push_back("coca");
    t1.alias.push_back("cola");
    t1.alias.push_back("coke");

    Term t2;
    t2.term = "1";
    t2.lowerCaseTerm = "1";
    t2.entityId = 1;
    t2.termId = 1;
    t2.alias.push_back("un");
    t2.alias.push_back("une");

    EXPECT_EQ(t1, dictionaryModel.dictionary.findTerm(0));
    EXPECT_EQ(t2, dictionaryModel.dictionary.findTerm(4));
}


TEST(IntentDictionaryDeserializerTest, test_simple_deserialization_from_json_string)
{
    const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

    std::string jsonContent = resourceManager.getResource(test::ResourceManager::ResourceId::
                                                          INTENT_DICTIONARY_DESERIALIZATION_JSON_EXAMPLE);

    nlohmann::json json = nlohmann::json::parse(jsonContent);

    Deserializer deserializer;
    IntentServiceModel intentServiceModel = deserializer.deserialize<IntentServiceModel>(json);

    test_simple_deserialization(*intentServiceModel.dictionaryModel, *intentServiceModel.intentModel);
}

TEST(IntentDictionaryDeserializerTest, test_simple_deserialization_from_invalid_istream)
{
    const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

    std::string jsonContent = resourceManager.getResource(test::ResourceManager::ResourceId::
                                                          INTENT_DICTIONARY_DESERIALIZATION_INVALID_JSON_EXAMPLE);

    bool exceptionCatched = false;
    try {
        std::stringstream ss;
        ss << jsonContent;

        Deserializer deserializer;
        IntentServiceModel intentServiceModel = deserializer.deserialize<IntentServiceModel>(ss);
    }
    catch (...)
    {
        exceptionCatched = true;
    }
    ASSERT_TRUE(exceptionCatched);
}

TEST(IntentDictionaryDeserializerTest, test_simple_deserialization_from_istream)
{
    const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

    std::string jsonContent = resourceManager.getResource(test::ResourceManager::ResourceId::
                                                          INTENT_DICTIONARY_DESERIALIZATION_JSON_EXAMPLE);

    std::stringstream ss;
    ss << jsonContent;

    Deserializer deserializer;
    IntentServiceModel intentServiceModel = deserializer.deserialize<IntentServiceModel>(ss);

    test_simple_deserialization(*intentServiceModel.dictionaryModel, *intentServiceModel.intentModel);
}

TEST(IntentDictionaryDeserializerTest, test_intent_story_deserialization_from_istream)
{
    const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

    std::string jsonContent = resourceManager.getResource(test::ResourceManager::ResourceId::
                                                          INTENT_DICTIONARY_DESERIALIZATION_JSON_EXAMPLE);

    std::stringstream ss;
    ss << jsonContent;

    Deserializer deserializer;
    IntentStoryServiceModel intentStoryServiceModel = deserializer.deserialize<IntentStoryServiceModel>(ss);

    ASSERT_EQ_SIGNED(6, intentStoryServiceModel.intentStoryModel->vertexByStateId.size());
    ASSERT_EQ_SIGNED(6, intentStoryServiceModel.intentStoryModel->graph.vertexCount());
    ASSERT_EQ_SIGNED(3, intentStoryServiceModel.intentStoryModel->graph.edgeCount());

    ASSERT_THAT(intentStoryServiceModel.intentStoryModel->terminalStateIds, SizeIs(2));

    ASSERT_EQ("root", intentStoryServiceModel.intentStoryModel->rootStateId);
}


TEST(IntentDictionaryDeserializerTest, test_chatbot_deserialization)
{
    const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

    std::string jsonContent = resourceManager.getResource(test::ResourceManager::ResourceId::
                                                          CHATBOT_MODE_JSON);

    std::stringstream ss;

    ChatbotActionModel chatbotActionModel;
    Deserializer deserializer;

    ss.str(jsonContent);
    ChatbotModel chatbotModel = deserializer.deserialize<ChatbotModel>(ss);
    chatbotActionModel = *chatbotModel.chatbotActionModel;

    EXPECT_THAT(chatbotActionModel.replyContentByReplyIdIndex, SizeIs(8));

    ASSERT_THAT(chatbotActionModel.replyIdsByActionId, SizeIs(7));

    EXPECT_THAT(chatbotActionModel.replyIdsByActionId["wake"],
                ElementsAre("what_can_i_get_you_reply"));

    EXPECT_THAT(chatbotActionModel.replyIdsByActionId["append_order1"],
                ElementsAre("want_something_else_reply"));

    EXPECT_THAT(chatbotActionModel.replyIdsByActionId["append_order2"],
                ElementsAre("want_something_else_reply"));

    EXPECT_THAT(chatbotActionModel.replyIdsByActionId["bye"],
                ElementsAre("bye_reply"));

    EXPECT_THAT(chatbotActionModel.replyIdsByActionId["grab_it"],
                ElementsAre("grab_it_reply", "bye_reply"));

}
