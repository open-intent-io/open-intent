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
// Created by clement on 29/05/16.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../launcher/TestContext.hpp"
#include "json.hpp"

#include "intent/utils/Deserializer.hpp"
#include "intent/chatbot/SingleSessionChatbot.hpp"

#include "mock/ChatbotMock.hpp"

using namespace ::testing;

namespace intent
{
    namespace test
    {
        class ChatbotTest : public ::testing::Test
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

        TEST_F(ChatbotTest, do_not_match_any_intent)
        {
            ReplyHandlerMock *replyHandlerMock = new NiceMock<ReplyHandlerMock>();
            Chatbot::ReplyActionHandler::SharedPtr replyHandler(replyHandlerMock);

            Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(
                    new NiceMock<UserDefinedCommandMock>());

            EXPECT_CALL(*replyHandlerMock, reply("Je ne comprends pas.")).Times(1);

            SingleSessionChatbot chatbot(m_chatbotModel, replyHandler, userDefinedActionHandler);

            chatbot.treatMessage("Paulo");
        }


        TEST_F(ChatbotTest, returns_the_right_initial_state)
        {
            Chatbot chatbot(m_chatbotModel);

            EXPECT_EQ("init", chatbot.getInitialState());
        }

        TEST_F(ChatbotTest, returns_the_right_terminal_states)
        {
            Chatbot chatbot(m_chatbotModel);

            EXPECT_THAT(chatbot.getTerminalStates(), UnorderedElementsAre("bye", "grab_it"));
        }
    }
}
