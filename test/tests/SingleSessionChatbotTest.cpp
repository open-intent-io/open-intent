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

#include "intent/utils/Deserializer.hpp"
#include "intent/chatbot/SingleSessionChatbot.hpp"

#include "mock/ChatbotMock.hpp"

using namespace ::testing;

namespace intent
{
    namespace test
    {
        class OrderSingleChatbotTest : public ::testing::Test
        {
        public:
            void SetUp()
            {
                const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

                std::string jsonContent = resourceManager.getResource(test::ResourceManager::ResourceId::CHATBOT_MODE_JSON);

                nlohmann::json json = nlohmann::json::parse(jsonContent);

                Deserializer deserializer;
                m_chatbotModel = deserializer.deserialize<ChatbotModel>(json);
            }

            ChatbotModel m_chatbotModel;
        };

        TEST_F(OrderSingleChatbotTest, reply_are_handled_correctly)
        {
            Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(
                    new NiceMock<UserDefinedCommandMock>());

            SingleSessionChatbot chatbot(m_chatbotModel, userDefinedActionHandler);

            std::vector<std::string> replies = chatbot.treatMessage("Bob!");

            EXPECT_THAT(replies, ElementsAre("Que puis-je vous offrir ?"));
        }

        TEST_F(OrderSingleChatbotTest, actions_are_handled_correctly)
        {
            UserDefinedCommandMock *userDefinedCommandMock = new NiceMock<UserDefinedCommandMock>();
            Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(userDefinedCommandMock);

            SingleSessionChatbot chatbot(m_chatbotModel, userDefinedActionHandler);

            EXPECT_CALL(*userDefinedCommandMock, execute("wake", _, _));
            EXPECT_CALL(*userDefinedCommandMock, execute("append_order1", _, _));
            EXPECT_CALL(*userDefinedCommandMock, execute("grab_it", _, _));

            chatbot.treatMessage("Bob!");
            chatbot.treatMessage("Je voudrais un Coca, s'il te plaît.");
            chatbot.treatMessage("Rien");
        }


        TEST_F(OrderSingleChatbotTest, add_items_twice_during_order)
        {
            UserDefinedCommandMock *userDefinedCommandMock = new NiceMock<UserDefinedCommandMock>();
            Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(userDefinedCommandMock);

            SingleSessionChatbot chatbot(m_chatbotModel, userDefinedActionHandler);

            EXPECT_CALL(*userDefinedCommandMock, execute("wake", _, _));
            EXPECT_CALL(*userDefinedCommandMock, execute("append_order1", _, _)).Times(2);
            EXPECT_CALL(*userDefinedCommandMock, execute("grab_it", _, _));

            chatbot.treatMessage("Bob!");
            chatbot.treatMessage("Je voudrais un Coca, s'il te plaît.");
            chatbot.treatMessage("Avec une kro!");
            chatbot.treatMessage("rien");
        }

        TEST_F(OrderSingleChatbotTest, fallback_reply_when_do_not_understand_an_intent)
        {
            UserDefinedCommandMock *userDefinedCommandMock = new NiceMock<UserDefinedCommandMock>();
            Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(userDefinedCommandMock);

            SingleSessionChatbot chatbot(m_chatbotModel, userDefinedActionHandler);


            chatbot.treatMessage("Bob!");
            std::vector<std::string> reply2 = chatbot.treatMessage("Je veux un camion de pompier");

            EXPECT_THAT(reply2, ElementsAre("Je ne comprends pas."));

            std::vector<std::string> reply3 = chatbot.treatMessage("Tu ne me comprends pas?");

            EXPECT_THAT(reply3, ElementsAre("Je ne comprends pas."));
        }


        class OrderWithTemplateRepliesSingleChatbotTest : public ::testing::Test
        {
        public:
            void SetUp()
            {
                const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

                std::string jsonContent = resourceManager.getResource(
                        test::ResourceManager::ResourceId::TEMPLATE_REPLIES_JSON);

                nlohmann::json json = nlohmann::json::parse(jsonContent);

                Deserializer deserializer;
                m_chatbotModel = deserializer.deserialize<ChatbotModel>(json);
            }

            ChatbotModel m_chatbotModel;
        };

        void publishVariables(const std::string &action, const Chatbot::VariablesMap &immutableVariables,
                              Chatbot::VariablesMap &mutableVariables)
        {
            if(action == "grab_it")
            {
                mutableVariables.insert(std::make_pair("price", "10.5€"));
            }
            if(action == "price_action")
            {
                Chatbot::VariablesMap::const_iterator it = immutableVariables.find("@price0");
                if (it != immutableVariables.end());
                    EXPECT_EQ("9€", it->second);
            }
        }

        TEST_F(OrderWithTemplateRepliesSingleChatbotTest, test_display_published_variables)
        {
            UserDefinedCommandMock *userDefinedCommandMock = new NiceMock<UserDefinedCommandMock>();
            Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(userDefinedCommandMock);

            SingleSessionChatbot chatbot(m_chatbotModel, userDefinedActionHandler);

            ON_CALL(*userDefinedCommandMock, execute(_, _, _)).WillByDefault(Invoke(publishVariables));

            std::vector<std::string> reply1 = chatbot.treatMessage("Bob!");
            std::vector<std::string> reply2 = chatbot.treatMessage("Je veux un Coca et deux Hein");
            std::vector<std::string> reply3 = chatbot.treatMessage("Rien");

            EXPECT_THAT(reply1, ElementsAre("Que puis-je vous offrir ?"));
            EXPECT_THAT(reply2,
                        ElementsAre("J'ajoute 1 Coca-Cola et 2 Heineken. Vous-voulez quelque chose d'autre ?"));
            EXPECT_THAT(reply3,
                        ElementsAre("Veuillez récupérer vos consommations au bar. Vous devrez payer 10.5€.",
                                    "Au revoir et à bientôt."));
        }



        class QueryChatbotTest : public ::testing::Test
        {
        public:
            void SetUp()
            {
                const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

                std::string jsonContent = resourceManager.getResource(test::ResourceManager::ResourceId::QUERY_MODE_JSON);

                nlohmann::json json = nlohmann::json::parse(jsonContent);

                Deserializer deserializer;
                m_chatbotModel = deserializer.deserialize<ChatbotModel>(json);
            }

            ChatbotModel m_chatbotModel;
        };


        TEST_F(QueryChatbotTest, test_intents_recognized_and_actions_called)
        {
            UserDefinedCommandMock *userDefinedCommandMock = new NiceMock<UserDefinedCommandMock>();
            Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(userDefinedCommandMock);

            SingleSessionChatbot chatbot(m_chatbotModel, userDefinedActionHandler);

            EXPECT_CALL(*userDefinedCommandMock, execute("price_action",_,_));
            EXPECT_CALL(*userDefinedCommandMock, execute("beverage_action",_,_));
            EXPECT_CALL(*userDefinedCommandMock, execute("category_action",_,_));

            chatbot.treatMessage("boissons pour moins de 9 €");
            chatbot.treatMessage("ok");
            chatbot.treatMessage("Combien pour une Kro ?");
            chatbot.treatMessage("ok");
            chatbot.treatMessage("Quels soft pour moins de 9 €?");
        }

        void pushQueryResult(const std::string &action, const Chatbot::VariablesMap &intentVariables,
                             Chatbot::VariablesMap &userDefinedVariables)
        {
            if(action == "beverage_action")
            {
                userDefinedVariables["beverages"] = "coca 5€ \n eau minérale 4€ \n";
            }
            else if(action == "category_action")
            {
                userDefinedVariables["beverages"] = "coca 5€ \n eau minérale 4€ \n";
            }
            else if(action == "price_action")
            {
                userDefinedVariables["price"] = "10.5";
            }
        }

        TEST_F(QueryChatbotTest, test_correct_replies)
        {
            UserDefinedCommandMock *userDefinedCommandMock = new NiceMock<UserDefinedCommandMock>();
            Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(userDefinedCommandMock);

            SingleSessionChatbot chatbot(m_chatbotModel, userDefinedActionHandler);

            ON_CALL(*userDefinedCommandMock, execute(_, _, _)).WillByDefault(Invoke(pushQueryResult));

            std::vector<std::string> reply1 = chatbot.treatMessage("boissons pour moins de 9 €");

            EXPECT_THAT(reply1,
                        ElementsAre("Voici les boissons : coca 5€ \n eau minérale 4€ \n"));

            std::vector<std::string> reply2 = chatbot.treatMessage("Combien pour une Kro ?");

            EXPECT_THAT(reply2,
                        ElementsAre("Le prix est : 10.5€."));

            std::vector<std::string> reply3 = chatbot.treatMessage("Quels soft pour moins de 9 €?");

            EXPECT_THAT(reply3,
                        ElementsAre("Voici les boissons : coca 5€ \n eau minérale 4€ \n"));
        }
    }
}
