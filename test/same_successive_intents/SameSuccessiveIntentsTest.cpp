#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "launcher/TestContext.hpp"
#include "json.hpp"

#include "intent/utils/Deserializer.hpp"
#include "intent/chatbot/SingleSessionChatbot.hpp"
#include "intent/interpreter/Interpreter.hpp"

#include "mock/ChatbotMock.hpp"

using namespace ::testing;

namespace intent
{
    namespace test
    {
        class SameSuccessiveIntentsTest : public ::testing::Test
        {
        public:
            void SetUp()
            {
                const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

                std::string dictionary = resourceManager.read("same_successive_intents/res/dictionary.json");
                std::string script = resourceManager.read("same_successive_intents/res/script.txt");

                std::stringstream ss;
                ss << dictionary;

                Deserializer deserializer;
                m_chatbotModel.intentStoryServiceModel.intentServiceModel.dictionaryModel =
                        deserializer.deserialize<DictionaryModel::SharedPtr>(ss);

                //finally we build the model
                InterpreterFeedback feedback;
                m_chatbotModel = Interpreter::build(
                        script, m_chatbotModel.intentStoryServiceModel.intentServiceModel.dictionaryModel, feedback);
            }

            ChatbotModel m_chatbotModel;
        };


        TEST_F(SameSuccessiveIntentsTest, do_not_skip_any_intent)
        {
            Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(
                    new NiceMock<UserDefinedCommandMock>());

            SingleSessionChatbot chatbot(m_chatbotModel, userDefinedActionHandler);

            std::vector<std::string> reply1 = chatbot.treatMessage("Salut");
            std::vector<std::string> reply2 = chatbot.treatMessage("Coucou");
            std::vector<std::string> reply3 = chatbot.treatMessage("Coucou");

            EXPECT_THAT(reply1, ElementsAre("Salut, je suis dans l'état 1"));
            EXPECT_THAT(reply2, ElementsAre("Salut, je suis dans l'état 2"));
            EXPECT_THAT(reply3, ElementsAre("Salut, je suis dans l'état 1"));
        }
    }
}

