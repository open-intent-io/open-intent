//
// Created by clement on 05/05/16.
//

#include "TestContext.hpp"

namespace intent
{
    namespace test
    {
        const intent::test::TestContext *gTestContext = NULL;

        TestContext::TestContext(const std::string &resourceDirectory)
            : m_resourceManager(resourceDirectory)
        {

        }

        void TestContext::SetUp()
        {
            try
            {
                registerResources();
            }
            catch(const intent::test::ResourceManagerException &e)
            {
                std::cout << e.message() << std::endl;
                throw e;
            }
        }

        void TestContext::TearDown()
        {

        }

        void TestContext::registerResources()
        {
            m_resourceManager.registerFile(ResourceManager::ResourceId::INTENT_DICTIONARY_DESERIALIZATION_JSON_EXAMPLE,
                                           "intent_dictionary_deserialization_test.json");
            m_resourceManager.registerFile(ResourceManager::ResourceId::ORDER_BEVERAGE_INTENT_JSON,
                                           "order_beverage_intent.json");
            m_resourceManager.registerFile(ResourceManager::ResourceId::CHATBOT_MODE_JSON,
                                           "chatbot_model.json");
            m_resourceManager.registerFile(ResourceManager::ResourceId::QUERY_MODE_JSON, "queries_model.json");
            m_resourceManager.registerFile(ResourceManager::ResourceId::TEMPLATE_REPLIES_JSON,
                                           "template_replies.json");
            m_resourceManager.registerFile(ResourceManager::ResourceId::INTERPRETER_MODEL,
                                           "interpreter_model.txt");
            m_resourceManager.registerFile(ResourceManager::ResourceId::INTERPRETER_MODEL_W_ERRORS,
                                           "interpreter_model_w_errors.txt");
            m_resourceManager.registerFile(ResourceManager::ResourceId::CHATBOT_MODE_JSON_WITHOUT_INTENT_STORY,
                                           "chatbot_model_without_intent_story.json");
        }

    }
}
