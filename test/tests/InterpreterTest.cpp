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
// Created by sergei on 05/05/16.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../launcher/TestContext.hpp"
#include "intent/intent_story_service/IntentStoryService.hpp"
#include "intent/interpreter/SentenceToIntentTranslator.hpp"
#include "intent/interpreter/EdgeParser.hpp"
#include "intent/interpreter/ReplyTemplateInterpreter.hpp"
#include "intent/chatbot/SingleSessionChatbot.hpp"
#include "mock/ChatbotMock.hpp"
#include "intent/intent_service/IntentEncoder.hpp"


#include "intent/interpreter/Interpreter.hpp"
#include "intent/utils/Deserializer.hpp"

#include <string>
#include <sstream>

using namespace intent;
using namespace ::testing;

#define EXPECT_EQ_SIGNED(v1, v2) EXPECT_EQ(v1, static_cast<int>(v2))
#define ASSERT_EQ_SIGNED(v1, v2) ASSERT_EQ(v1, static_cast<int>(v2))

namespace intent { namespace test {

class InterpreterTest : public ::testing::Test
{
public:
    void SetUp()
    {
        const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

        std::string intentDictionaryContent = resourceManager.getResource(test::ResourceManager::ResourceId::
                                                              INTENT_DICTIONARY_DESERIALIZATION_JSON_EXAMPLE);

        std::stringstream ss;
        ss << intentDictionaryContent;

        Deserializer deserializer;
        IntentStoryServiceModel intentStoryServiceModel = deserializer.deserialize<IntentStoryServiceModel>(ss);

        m_dictionaryModel = *intentStoryServiceModel.intentServiceModel.dictionaryModel;
    }
    DictionaryModel m_dictionaryModel;
};


TEST_F(InterpreterTest, check_that_intent_is_defined_by_entities)
{
    std::pair<IntentModel::IndexType, IntentModel::Intent> intent =
            SentenceToIntentTranslator::translate("Je voudrais une Kronenbourg", m_dictionaryModel);
    EXPECT_EQ(IntentEncoder::encode({1,0}), intent.first);

    intent =
            SentenceToIntentTranslator::translate("Je voudrais une Kronenbourg et un coca", m_dictionaryModel);
    EXPECT_EQ(IntentEncoder::encode({1,0,1,0}), intent.first);
}

TEST_F(InterpreterTest, check_that_an_edge_is_parsed)
{
    const Scenario lines = {ScriptLine("@root"),
                            ScriptLine("-je voudrais un coca"),
                            ScriptLine("#action"),
                            ScriptLine("-je vous en prie"),
                            ScriptLine("@end")};
    const std::pair<int,int> inquiryToReply(1,3);

    int vertexCount = 0;
    std::unique_ptr<std::string> previousState;
    InterpreterFeedback interpreterFeedback;
    EdgeParser edgeParser(m_dictionaryModel, vertexCount, interpreterFeedback);

    EdgeDefinition edge = edgeParser.parse(lines, inquiryToReply, previousState);

    EXPECT_EQ("@root", edge.source.stateId);
    EXPECT_EQ("@end", edge.target.stateId);
    EXPECT_EQ("#action", edge.edge.actionId);
    EXPECT_EQ(IntentEncoder::encode({1,0}), edge.edge.intentId);
}

TEST_F(InterpreterTest, check_that_an_edge_is_parsed_wout_annot)
{
    const Scenario lines = {ScriptLine("-je voudrais un coca"), ScriptLine("-je vous en prie")};
    const std::pair<int,int> inquiryToReply(0,1);

    int vertexCount = 0;
    std::unique_ptr<std::string> previousState;
    InterpreterFeedback interpreterFeedback;
    EdgeParser edgeParser(m_dictionaryModel, vertexCount, interpreterFeedback);

    EdgeDefinition edge = edgeParser.parse(lines, inquiryToReply, previousState);

    EXPECT_EQ("@anonymous_state0", edge.source.stateId);
    EXPECT_EQ("@anonymous_state1", edge.target.stateId);
    EXPECT_EQ("", edge.edge.actionId);
    EXPECT_EQ(IntentEncoder::encode({1,0}), edge.edge.intentId);
}


TEST_F(InterpreterTest, check_that_an_inquiry_is_parsed_with_regexp)
{
    const Scenario lines = {ScriptLine("-je voudrais (un coca)*"),
                            ScriptLine("-je vous en prie")};

    const std::pair<int,int> inquiryToReply(0,1);

    int vertexCount = 0;
    std::unique_ptr<std::string> previousState;
    InterpreterFeedback interpreterFeedback;
    EdgeParser edgeParser(m_dictionaryModel, vertexCount, interpreterFeedback);

    EdgeDefinition edge = edgeParser.parse(lines, inquiryToReply, previousState);
    EXPECT_EQ(IntentEncoder::encode({0xFFFD,1,0,0xFFFC,0xFFFA}), edge.edge.intentId);
}

TEST_F(InterpreterTest, check_that_successive_edges_are_parsed)
{
    const Scenario lines = {ScriptLine("@root"),
                            ScriptLine("-je voudrais un coca"),
                            ScriptLine("#action"),
                            ScriptLine("-je vous en prie"),
                            ScriptLine("@intermediary"),
                            ScriptLine("-et une kro et une Hein"),
                            ScriptLine("#other_action"),
                            ScriptLine("-c'est noté"),
                            ScriptLine("@end")};

    const std::pair<int,int> inquiryToReply1(1, 3);

    int vertexCount = 0;
    std::unique_ptr<std::string> previousState;
    InterpreterFeedback interpreterFeedback;
    EdgeParser edgeParser(m_dictionaryModel, vertexCount, interpreterFeedback);

    EdgeDefinition edge1 = edgeParser.parse(lines, inquiryToReply1, previousState);
    EXPECT_EQ("@root", edge1.source.stateId);
    EXPECT_EQ("@intermediary", edge1.target.stateId);
    EXPECT_EQ("#action", edge1.edge.actionId);
    EXPECT_EQ(IntentEncoder::encode({1,0}), edge1.edge.intentId);

    const std::pair<int,int> inquiryToReply2(5, 7);

    EdgeDefinition edge2 = edgeParser.parse(lines, inquiryToReply2, previousState);
    EXPECT_EQ("@intermediary", edge2.source.stateId);
    EXPECT_EQ("@end", edge2.target.stateId);
    EXPECT_EQ("#other_action", edge2.edge.actionId);
    EXPECT_EQ(IntentEncoder::encode({1,0,1,0}), edge2.edge.intentId);
}

TEST_F(InterpreterTest, check_that_response_template_is_adapted)
{
    std::string replyTemplate = "Ca vous fera _ et _ et _ et _ et enfin _";
    ReplyTemplateInterpreter::adapt(replyTemplate);
    ASSERT_EQ("Ca vous fera ${0} et ${1} et ${2} et ${3} et enfin ${4}", replyTemplate);
}


typedef std::map<std::string, float> Menu;
typedef std::map<std::string, int> Cart;
class InterpreterWithChatbotTest : public ::testing::Test
{
public:
    void SetUp()
    {
        const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

        std::string chatbotModelFileContent = resourceManager.getResource(test::ResourceManager::ResourceId::
                                                              CHATBOT_MODE_JSON_WITHOUT_INTENT_STORY);
        std::stringstream ss;
        ss << chatbotModelFileContent;

        Deserializer deserializer;
        m_chatbotModel.intentStoryServiceModel.intentServiceModel.dictionaryModel =
                deserializer.deserialize<DictionaryModel::SharedPtr>(ss);

        //prepare file content
        std::string fileContent = resourceManager.getResource(test::ResourceManager::ResourceId::
                                                              INTERPRETER_MODEL);

        //finally we build the model
        InterpreterFeedback feedback;
        m_chatbotModel = Interpreter::build(
                fileContent, m_chatbotModel.intentStoryServiceModel.intentServiceModel.dictionaryModel, feedback);
    }

    ChatbotModel m_chatbotModel;
};

void pushVariables(const std::string &action, const Chatbot::VariablesMap &immutableVariables,
                      Chatbot::VariablesMap &mutableVariables)
{
    if(action == "#grab_it")
    {
        mutableVariables.insert(std::make_pair("0", "10.5€"));
    }
}

TEST_F(InterpreterWithChatbotTest, test_full_scenario)
{
    UserDefinedCommandMock *userDefinedCommandMock = new NiceMock<UserDefinedCommandMock>();
    Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(userDefinedCommandMock);

    SingleSessionChatbot chatbot(m_chatbotModel, userDefinedActionHandler);

    ON_CALL(*userDefinedCommandMock, execute(_, _, _)).WillByDefault(Invoke(pushVariables));

    std::vector<std::string> reply1 = chatbot.treatMessage("Bob!");
    std::vector<std::string> reply2 = chatbot.treatMessage("Je veux un Coca et deux Hein");
    std::vector<std::string> reply3 = chatbot.treatMessage("Rien");

    EXPECT_THAT(reply1, ElementsAre("Que puis-je vous offrir ?"));
    EXPECT_THAT(reply2,
                ElementsAre("Vous-voulez quelque chose d'autre ?"));
    EXPECT_THAT(reply3,
                ElementsAre("Veuillez récupérer vos consommations au bar. Vous devrez payer 10.5€."));
}


TEST_F(InterpreterWithChatbotTest, test_fallback_reply_on_edges)
{
    UserDefinedCommandMock *userDefinedCommandMock = new NiceMock<UserDefinedCommandMock>();
    Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(userDefinedCommandMock);

    SingleSessionChatbot chatbot(m_chatbotModel, userDefinedActionHandler);

    ON_CALL(*userDefinedCommandMock, execute(_, _, _)).WillByDefault(Invoke(pushVariables));

    std::vector<std::string> reply1 = chatbot.treatMessage("Je veux un Coca et deux Hein");
    std::vector<std::string> reply2 = chatbot.treatMessage("Bob!");
    std::vector<std::string> reply3 = chatbot.treatMessage("Blabla");

    EXPECT_THAT(reply1, ElementsAre("Je n'ai pas compris votre demande"));
    EXPECT_THAT(reply2, ElementsAre("Que puis-je vous offrir ?"));
    EXPECT_THAT(reply3,
                ElementsAre("Soyez le plus précis possible"));
}

class InterpreterFeedbackTest : public ::testing::Test
{
public:
    void SetUp()
    {
        const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

        std::string intentDictionaryContent = resourceManager.getResource(test::ResourceManager::ResourceId::
                                                              INTENT_DICTIONARY_DESERIALIZATION_JSON_EXAMPLE);

        std::stringstream ss;
        ss << intentDictionaryContent;

        Deserializer deserializer;
        IntentStoryServiceModel intentStoryServiceModel = deserializer.deserialize<IntentStoryServiceModel>(ss);

        m_dictionaryModel = *intentStoryServiceModel.intentServiceModel.dictionaryModel;
    }
    DictionaryModel m_dictionaryModel;
};


TEST_F(InterpreterFeedbackTest, check_root_and_terminal_state_msg)
{
    const Scenario lines = {ScriptLine("_no_state_", 0),
                            ScriptLine("-je voudrais un coca", 1),
                            ScriptLine("#action", 2),
                            ScriptLine("-je vous en prie", 3),
                            ScriptLine("_no_state_", 4)};
    const std::pair<int,int> inquiryToReply(1,3);

    int vertexCount = 0;
    std::unique_ptr<std::string> previousState;
    InterpreterFeedback interpreterFeedback;
    EdgeParser edgeParser(m_dictionaryModel, vertexCount, interpreterFeedback);

    EdgeDefinition edge = edgeParser.parse(lines, inquiryToReply, previousState);

    EXPECT_EQ_SIGNED(2, interpreterFeedback.size());
    EXPECT_EQ(messagesText[ANONYMOUS_STATE_CREATION], interpreterFeedback[0].message);
    EXPECT_EQ("-je voudrais un coca", interpreterFeedback[0].line.content);
    EXPECT_EQ_SIGNED(1, interpreterFeedback[0].line.position);
    EXPECT_EQ(INFO, interpreterFeedback[0].level);

    EXPECT_EQ(messagesText[ANONYMOUS_STATE_CREATION], interpreterFeedback[1].message);
    EXPECT_EQ("-je vous en prie", interpreterFeedback[1].line.content);
    EXPECT_EQ_SIGNED(3, interpreterFeedback[1].line.position);
    EXPECT_EQ(INFO, interpreterFeedback[1].level);
}


TEST_F(InterpreterFeedbackTest, check_no_action_warning)
{
    const Scenario lines = {ScriptLine("@root", 0),
                            ScriptLine("-je voudrais un coca", 1),
                            ScriptLine("-je vous en prie", 2),
                            ScriptLine("@end", 3)};
    const std::pair<int,int> inquiryToReply(1,2);

    int vertexCount = 0;
    std::unique_ptr<std::string> previousState;
    InterpreterFeedback interpreterFeedback;
    EdgeParser edgeParser(m_dictionaryModel, vertexCount, interpreterFeedback);

    EdgeDefinition edge = edgeParser.parse(lines, inquiryToReply, previousState);

    EXPECT_EQ_SIGNED(1, interpreterFeedback.size());
    EXPECT_EQ(messagesText[NO_ACTION], interpreterFeedback[0].message);
    EXPECT_EQ("-je vous en prie", interpreterFeedback[0].line.content);
    EXPECT_EQ_SIGNED(2, interpreterFeedback[0].line.position);
    EXPECT_EQ(WARNING, interpreterFeedback[0].level);
}

TEST_F(InterpreterFeedbackTest, check_no_entity_warning)
{
    const Scenario lines = {ScriptLine("@root", 0),
                            ScriptLine("-je voudrais dix chameaux", 1),
                            ScriptLine("#action", 2),
                            ScriptLine("-je vous en prie", 3),
                            ScriptLine("@end", 4)};
    const std::pair<int,int> inquiryToReply(1,3);

    int vertexCount = 0;
    std::unique_ptr<std::string> previousState;
    InterpreterFeedback interpreterFeedback;
    EdgeParser edgeParser(m_dictionaryModel, vertexCount, interpreterFeedback);

    EdgeDefinition edge = edgeParser.parse(lines, inquiryToReply, previousState);

    EXPECT_EQ_SIGNED(1, interpreterFeedback.size());
    EXPECT_EQ(messagesText[NO_ENTITY], interpreterFeedback[0].message);
    EXPECT_EQ("-je voudrais dix chameaux", interpreterFeedback[0].line.content);
    EXPECT_EQ_SIGNED(1, interpreterFeedback[0].line.position);
    EXPECT_EQ(WARNING, interpreterFeedback[0].level);
}

class InterpreterIncorrectModelTest : public ::testing::Test
{
public:
    void SetUp()
    {
        const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

        std::string chatbotModelFileContent = resourceManager.getResource(test::ResourceManager::ResourceId::
                                                              CHATBOT_MODE_JSON_WITHOUT_INTENT_STORY);
        std::stringstream ss;
        ss << chatbotModelFileContent;

        Deserializer deserializer;
        ChatbotModel chatbotModel;
        chatbotModel.intentStoryServiceModel.intentServiceModel.dictionaryModel =
                deserializer.deserialize<DictionaryModel::SharedPtr>(ss);

        //prepare file content
        std::string fileContent = resourceManager.getResource(test::ResourceManager::ResourceId::
                                                              INTERPRETER_MODEL_W_ERRORS);

        //finally we build the model
        chatbotModel = Interpreter::build(
                fileContent, chatbotModel.intentStoryServiceModel.intentServiceModel.dictionaryModel, m_interpreterFeedback);
    }

    InterpreterFeedback m_interpreterFeedback;
};

TEST_F(InterpreterIncorrectModelTest, check_interpreter_feedback_when_wrong_model)
{
    EXPECT_EQ_SIGNED(3, m_interpreterFeedback.size());

    EXPECT_EQ(messagesText[ROOT_STATE_MSG], m_interpreterFeedback[0].message);
    EXPECT_EQ("_root", m_interpreterFeedback[0].line.content);
    EXPECT_EQ_SIGNED(1, m_interpreterFeedback[0].line.position);

    EXPECT_EQ(messagesText[ANONYMOUS_STATE_CREATION], m_interpreterFeedback[1].message);
    EXPECT_EQ("-Bob!", m_interpreterFeedback[1].line.content);
    EXPECT_EQ_SIGNED(2, m_interpreterFeedback[1].line.position);

    EXPECT_EQ(messagesText[NO_ACTION], m_interpreterFeedback[2].message);
    EXPECT_EQ("_wake", m_interpreterFeedback[2].line.content);
    EXPECT_EQ_SIGNED(3, m_interpreterFeedback[2].line.position);
}

}}

