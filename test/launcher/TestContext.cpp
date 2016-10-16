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
                                           "res/intent_dictionary_deserialization_test.json");
            m_resourceManager.registerFile(ResourceManager::ResourceId::INTENT_DICTIONARY_DESERIALIZATION_INVALID_JSON_EXAMPLE,
                                           "res/intent_dictionary_deserialization_invalid_test.json");
            m_resourceManager.registerFile(ResourceManager::ResourceId::ORDER_BEVERAGE_INTENT_JSON,
                                           "res/order_beverage_intent.json");
            m_resourceManager.registerFile(ResourceManager::ResourceId::CHATBOT_MODEL_JSON,
                                           "res/chatbot_model.json");
            m_resourceManager.registerFile(ResourceManager::ResourceId::QUERY_MODE_JSON, "res/queries_model.json");
            m_resourceManager.registerFile(ResourceManager::ResourceId::TEMPLATE_REPLIES_JSON,
                                           "res/template_replies.json");
            m_resourceManager.registerFile(ResourceManager::ResourceId::INTERPRETER_MODEL,
                                           "res/interpreter_model.txt");
            m_resourceManager.registerFile(ResourceManager::ResourceId::INTERPRETER_MODEL_W_ERRORS,
                                           "res/interpreter_model_w_errors.txt");
            m_resourceManager.registerFile(ResourceManager::ResourceId::CHATBOT_MODEL_JSON_WITHOUT_INTENT_STORY,
                                           "res/chatbot_model_without_intent_story.json");
        }

    }
}
