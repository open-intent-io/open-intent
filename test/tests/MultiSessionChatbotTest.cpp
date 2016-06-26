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
// Created by clement on 14/05/16.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../launcher/TestContext.hpp"

#include "json.hpp"

#include "intent/utils/Deserializer.hpp"
#include "intent/chatbot/MultiSessionChatbot.hpp"

#include "mock/ChatbotMock.hpp"


#define EXPECT_EQ_SIGNED(v1, v2) EXPECT_EQ(v1, static_cast<int>(v2))
#define ASSERT_EQ_SIGNED(v1, v2) ASSERT_EQ(v1, static_cast<int>(v2))

using namespace ::testing;

namespace intent
{
    namespace test
    {
        class OrderMultiSessionChatbotTest : public ::testing::Test
        {
        public:
            void SetUp()
            {
                const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

                std::string jsonContent = resourceManager.getResource(
                        test::ResourceManager::ResourceId::CHATBOT_MODE_JSON);

                nlohmann::json json = nlohmann::json::parse(jsonContent);

                Deserializer deserializer;
                m_chatbotModel = deserializer.deserialize<ChatbotModel>(json);
            }

            ChatbotModel m_chatbotModel;
        };

        TEST_F(OrderMultiSessionChatbotTest, reply_are_handled_correctly)
        {
            typedef MultiSessionChatbot<std::string> MyChatbot;

            NiceMock< MultiSessionReplyHandlerMock > *replyHandlerMock =
                    new NiceMock< MultiSessionReplyHandlerMock >();
            MyChatbot::ReplyActionHandler::SharedPtr replyHandler(replyHandlerMock);


            MyChatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(
                    new NiceMock<MultiSessionUserDefinedCommandMock >());

            EXPECT_CALL(*replyHandlerMock, reply("sess1", "Que puis-je vous offrir ?"));
            EXPECT_CALL(*replyHandlerMock, reply("sess2", "Que puis-je vous offrir ?"));

            MyChatbot chatbot(m_chatbotModel, replyHandler, userDefinedActionHandler);

            chatbot.addSession("sess1");
            chatbot.addSession("sess2");

            chatbot.treatMessage("sess1", "Bob!");
            chatbot.treatMessage("sess2", "Bob!");
        }

        TEST_F(OrderMultiSessionChatbotTest, test_session_creation_and_deletion)
        {
            typedef MultiSessionChatbot<std::string> MyChatbot;

            NiceMock< MultiSessionReplyHandlerMock > *replyHandlerMock =
                    new NiceMock< MultiSessionReplyHandlerMock >();
            MyChatbot::ReplyActionHandler::SharedPtr replyHandler(replyHandlerMock);


            MyChatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(
                    new NiceMock<MultiSessionUserDefinedCommandMock >());

            MyChatbot chatbot(m_chatbotModel, replyHandler, userDefinedActionHandler);
            EXPECT_EQ_SIGNED(0, chatbot.sessionCount());

            chatbot.addSession("sess1");
            chatbot.addSession("sess2");
            EXPECT_EQ_SIGNED(2, chatbot.sessionCount());

            chatbot.removeSession("sess2");
            EXPECT_EQ_SIGNED(1, chatbot.sessionCount());

            chatbot.removeSession("sess3");
            EXPECT_EQ_SIGNED(1, chatbot.sessionCount());

            chatbot.removeSession("sess1");
            EXPECT_EQ_SIGNED(0, chatbot.sessionCount());

            chatbot.addSession("sess3");
            EXPECT_EQ_SIGNED(1, chatbot.sessionCount());
        }
    }
}
