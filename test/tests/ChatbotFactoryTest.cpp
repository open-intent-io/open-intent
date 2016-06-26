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
// Created by clement on 22/05/16.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "launcher/ResourceManager.hpp"
#include "launcher/TestContext.hpp"

#include "intent/chatbot/ChatbotFactory.hpp"

#include "mock/ChatbotMock.hpp"

#include <fstream>

namespace intent
{
    namespace test
    {
        using namespace ::testing;

        namespace
        {

            void pushVariables(const std::string &action, const Chatbot::VariablesMap &intentVariables,
                               Chatbot::VariablesMap &userDefinedVariables)
            {
                if (action == "#grab_it")
                {
                    userDefinedVariables.insert(std::make_pair("0", "10.5€"));
                }
            }
        }

        TEST(ChatbotFactoryTest, create_single_session_chatbot_from_oiml)
        {
            const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

            std::string interpreterPath = resourceManager.getResourcePath(
                    test::ResourceManager::ResourceId::INTERPRETER_MODEL);

            std::string dictionaryPath = resourceManager.getResourcePath(
                    test::ResourceManager::ResourceId::CHATBOT_MODE_JSON_WITHOUT_INTENT_STORY);

            NiceMock<ReplyHandlerMock> *replyHandlerMock = new NiceMock<ReplyHandlerMock>();
            Chatbot::ReplyActionHandler::SharedPtr replyHandler(replyHandlerMock);
            NiceMock<UserDefinedCommandMock> *userDefinedCommandMock = new NiceMock<UserDefinedCommandMock>();
            Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(userDefinedCommandMock);

            std::ifstream interpreterFile(interpreterPath);
            std::ifstream dictionaryFile(dictionaryPath);

            SingleSessionChatbot::SharedPtr chatbot =
                    ChatbotFactory::createSingleSessionChatbotFromOIML(dictionaryFile, interpreterFile,
                                                                       replyHandler, userDefinedActionHandler);

            ASSERT_THAT(chatbot, NotNull());

            ON_CALL(*userDefinedCommandMock, execute(_, _, _)).WillByDefault(Invoke(pushVariables));

            EXPECT_CALL(*replyHandlerMock, reply("Que puis-je vous offrir ?"));
            EXPECT_CALL(*replyHandlerMock, reply("Vous-voulez quelque chose d'autre ?"));
            EXPECT_CALL(*replyHandlerMock,
                        reply("Veuillez récupérer vos consommations au bar. Vous devrez payer 10.5€."));

            chatbot->treatMessage("Bob!");
            chatbot->treatMessage("Je veux un Coca et une Kro");
            chatbot->treatMessage("Rien");
        }

        TEST(ChatbotFactoryTest, create_single_session_chatbot_from_json_model)
        {
            const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

            std::string chatbotModelPath = resourceManager.getResourcePath(
                    test::ResourceManager::ResourceId::CHATBOT_MODE_JSON);

            NiceMock<ReplyHandlerMock> *replyHandlerMock = new NiceMock<ReplyHandlerMock>();
            Chatbot::ReplyActionHandler::SharedPtr replyHandler(replyHandlerMock);
            NiceMock<UserDefinedCommandMock> *userDefinedCommandMock = new NiceMock<UserDefinedCommandMock>();
            Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(userDefinedCommandMock);

            std::ifstream chatbotModelFile(chatbotModelPath);
            SingleSessionChatbot::SharedPtr chatbot =
                    ChatbotFactory::createSingleSessionChatbotFromJsonModel(chatbotModelFile,
                                                                            replyHandler, userDefinedActionHandler);

            ASSERT_THAT(chatbot, NotNull());

            ON_CALL(*userDefinedCommandMock, execute(_, _, _)).WillByDefault(Invoke(pushVariables));

            EXPECT_CALL(*replyHandlerMock, reply("Que puis-je vous offrir ?"));
            EXPECT_CALL(*replyHandlerMock, reply("Vous-voulez quelque chose d'autre ?"));
            EXPECT_CALL(*replyHandlerMock,
                        reply("Au revoir et à bientôt."));

            chatbot->treatMessage("Bob!");
            chatbot->treatMessage("Je veux un Coca et une Kro");
            chatbot->treatMessage("Rien");
        }

        TEST(ChatbotFactoryTest, create_chatbot_from_unexisting_file)
        {
            NiceMock<ReplyHandlerMock> *replyHandlerMock = new NiceMock<ReplyHandlerMock>();
            Chatbot::ReplyActionHandler::SharedPtr replyHandler(replyHandlerMock);
            NiceMock<UserDefinedCommandMock> *userDefinedCommandMock = new NiceMock<UserDefinedCommandMock>();
            Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(userDefinedCommandMock);

            std::ifstream unexistingFile("unexisting_file.json");

            SingleSessionChatbot::SharedPtr chatbot =
                    ChatbotFactory::createSingleSessionChatbotFromJsonModel(unexistingFile,
                                                                            replyHandler, userDefinedActionHandler);

            ASSERT_THAT(chatbot, IsNull());
        }




        namespace
        {
            void pushVariablesMultiSession(const std::string &sessionId, const std::string &action,
                                           const Chatbot::VariablesMap &intentVariables,
                                           Chatbot::VariablesMap &userDefinedVariables)
            {
                if (action == "#grab_it")
                {
                    userDefinedVariables.insert(std::make_pair("0", "10.5€"));
                }
            }
        }


        /* multi session */

        TEST(ChatbotFactoryTest, create_multi_session_chatbot_from_oiml)
        {
            const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

            std::string interpreterPath = resourceManager.getResourcePath(
                    test::ResourceManager::ResourceId::INTERPRETER_MODEL);

            std::string dictionaryPath = resourceManager.getResourcePath(
                    test::ResourceManager::ResourceId::CHATBOT_MODE_JSON_WITHOUT_INTENT_STORY);

            typedef MultiSessionChatbot<std::string> MyChatbot;
            NiceMock<MultiSessionReplyHandlerMock> *replyHandlerMock = new NiceMock<MultiSessionReplyHandlerMock>();
            MyChatbot::ReplyActionHandler::SharedPtr replyHandler(replyHandlerMock);
            NiceMock<MultiSessionUserDefinedCommandMock> *userDefinedCommandMock = new NiceMock<MultiSessionUserDefinedCommandMock>();
            MyChatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(userDefinedCommandMock);

            std::ifstream interpreterFile(interpreterPath);
            std::ifstream dictionaryFile(dictionaryPath);

            MyChatbot::SharedPtr chatbot =
                    ChatbotFactory::createMultiSessionChatbotFromOIML<std::string>(
                            dictionaryFile, interpreterFile, replyHandler, userDefinedActionHandler);

            ASSERT_THAT(chatbot, NotNull());

            ON_CALL(*userDefinedCommandMock, execute(_, _, _, _)).WillByDefault(Invoke(pushVariablesMultiSession));

            EXPECT_CALL(*replyHandlerMock, reply("123", "Que puis-je vous offrir ?"));
            EXPECT_CALL(*replyHandlerMock, reply("123", "Vous-voulez quelque chose d'autre ?"));
            EXPECT_CALL(*replyHandlerMock,
                        reply("123", "Veuillez récupérer vos consommations au bar. Vous devrez payer 10.5€."));

            EXPECT_CALL(*replyHandlerMock, reply("124", "Que puis-je vous offrir ?"));

            chatbot->addSession("123");
            chatbot->addSession("124");

            chatbot->treatMessage("123", "Bob!");
            chatbot->treatMessage("123", "Je veux un Coca et une Kro");
            chatbot->treatMessage("123", "Rien");

            chatbot->treatMessage("124", "Bob!");


            chatbot->treatMessage("125", "Bob!");
        }


    }
}
