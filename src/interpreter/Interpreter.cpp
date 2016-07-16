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
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "intent/interpreter/Interpreter.hpp"
#include "intent/intent_service/EntitiesMatcher.hpp"
#include "intent/interpreter/EdgeParser.hpp"
#include "intent/interpreter/SentenceToIntentTranslator.hpp"
#include "intent/interpreter/ReplyTemplateInterpreter.hpp"

#include <boost/algorithm/string.hpp>
#include <cassert>

#include <fstream>
#include <intent/chatbot/ChatbotModel.hpp>

namespace intent {

namespace {
typedef IntentModel::IndexType IndexType;
typedef IntentModel::Intent Intent;

const std::string DEFAULT_REPLY_ID = "#reply";

void indexScenario(const Scenario& scenario,
                   std::map<int, int>& inquiryToReply) {
  int index = 0;
  int counter = 0;
  std::pair<int, int> pair;
  std::for_each(
      scenario.begin(), scenario.end(),
      [&pair, &index, &counter, &inquiryToReply](const ScriptLine& line) {
        if (isLine<SAYING>(line)) {
          if (counter % 2 == 0) {
            pair.first = index;
          }
          if (counter % 2 == 1) {
            pair.second = index;
            inquiryToReply.insert(pair);
          }
          ++counter;
        }
        ++index;
      });
}

std::string extractSentence(const ScriptLine& scriptLine) {
  std::string content = scriptLine.content;
  assert(content.size() > 1);
  return content.substr(1, content.size() - 1);
}

struct IntentInserter {
  IntentInserter(const DictionaryModel& dictionaryModel,
                 const Scenario& scenario, IntentModel& intentModel)
      : m_dictionaryModel(dictionaryModel),
        m_intentModel(intentModel),
        m_scenario(scenario) {}

  void operator()(const std::pair<int, int> inquiryToReply) {
    std::string inquiry = extractSentence(m_scenario[inquiryToReply.first]);
    std::pair<IndexType, Intent> intent =
        SentenceToIntentTranslator::translate(inquiry, m_dictionaryModel);
    m_intentModel.intentsByIntentId.insert(intent);
  }

  IntentModel& m_intentModel;
  const Scenario& m_scenario;
  const DictionaryModel& m_dictionaryModel;
};

struct EdgeParserWrapper {
  EdgeParserWrapper(const Scenario& scenario,
                    std::unique_ptr<std::string>& previousStateInScenario,
                    EdgeParser& edgeParser)
      : m_scenario(scenario),
        m_previousStateInScenario(previousStateInScenario),
        m_edgeParser(edgeParser) {}

  EdgeDefinition operator()(const std::pair<int, int> inquiryToReply) {
    return m_edgeParser.parse(m_scenario, inquiryToReply,
                              m_previousStateInScenario);
  }

  const Scenario& m_scenario;
  EdgeParser& m_edgeParser;
  std::unique_ptr<std::string>& m_previousStateInScenario;
};

struct FallbackEdgesRetriever {
  FallbackEdgesRetriever(const Scenario& scenario,
                         std::unique_ptr<std::string>& previousStateInScenario,
                         EdgeParser& edgeParser,
                         std::vector<EdgeDefinition>& edgesToInsert)
      : m_scenario(scenario),
        m_previousStateInScenario(previousStateInScenario),
        m_edgeParser(edgeParser),
        m_edgesToInsert(edgesToInsert) {}

  void operator()(const std::pair<int, int> inquiryToReply) {
    std::unique_ptr<EdgeDefinition> edge;
    edge = m_edgeParser.parseFallback(m_scenario, inquiryToReply,
                                      m_previousStateInScenario);
    if (edge.get()) m_edgesToInsert.push_back(*edge);
  }

  const Scenario& m_scenario;
  EdgeParser& m_edgeParser;
  std::unique_ptr<std::string>& m_previousStateInScenario;
  std::vector<EdgeDefinition>& m_edgesToInsert;
};

struct ActionInserter {
  ActionInserter(ChatbotActionModel& chatbotActionModel, int& repliesCounter)
      : m_chatbotActionModel(chatbotActionModel),
        m_repliesCounter(repliesCounter) {}

  void operator()(EdgeDefinition& edge) {
    const std::string replyId = DEFAULT_REPLY_ID + "_" + edge.edge.actionId;

    ReplyTemplateInterpreter::adapt(edge.replyTemplate);
    m_chatbotActionModel.replyContentByReplyIdIndex[replyId] =
        edge.replyTemplate;

    if (m_chatbotActionModel.replyIdsByActionId[edge.edge.actionId].empty()) {
      m_chatbotActionModel.replyIdsByActionId[edge.edge.actionId].push_back(
          replyId);
    }
    ++m_repliesCounter;
  }

  int& m_repliesCounter;
  ChatbotActionModel& m_chatbotActionModel;
};

}  // anonymous

std::vector<ScriptLine> tokenizeInLines(const std::string& input) {
  std::vector<std::string> lines;
  std::vector<char> delimiters = {'\n'};
  Tokenizer::tokenize(input, delimiters, lines);
  std::vector<ScriptLine> scriptLines;
  for (unsigned int i = 0; i < lines.size(); ++i) {
    scriptLines.push_back({lines[i], i});
  }
  return scriptLines;
}

void extractScenarios(const std::string& script, Scenarios& scenarios) {
  int braceCounter = 0;
  Scenario scenario;
  std::vector<ScriptLine> lines = tokenizeInLines(script);

  std::for_each(lines.begin(), lines.end(),
                [&scenarios, &scenario, &braceCounter](ScriptLine& line) {
                  boost::trim(line.content);
                  if (isLine<CLOSE_SCENARIO>(line)) {
                    --braceCounter;
                  }
                  if (braceCounter == 1) {
                    scenario.push_back(line);
                  }
                  if (isLine<START_SCENARIO>(line)) {
                    ++braceCounter;
                  }
                  if (braceCounter == 0) {
                    if (!scenario.empty()) scenarios.push_back(scenario);
                    scenario.clear();
                  }

                });
}

void addEdgeDefinitionToModel(
    const EdgeDefinition& edge, IntentStoryModel& intentStoryModel,
    std::unordered_map<std::string, IntentStoryModel::StoryGraph::Vertex>&
        vertexIndex) {
  std::string sourceId = edge.source.stateId;
  std::string targetId = edge.target.stateId;

  // add source and target to vertex index if not there yet
  if (vertexIndex.find(sourceId) == vertexIndex.end())
    vertexIndex[sourceId] = intentStoryModel.graph.addVertex(edge.source);
  if (vertexIndex.find(targetId) == vertexIndex.end())
    vertexIndex[targetId] = intentStoryModel.graph.addVertex(edge.target);

  // add edge to graph
  intentStoryModel.graph.addEdge(vertexIndex[sourceId], vertexIndex[targetId],
                                 edge.edge);
  intentStoryModel.vertexByStateId[sourceId] = vertexIndex[sourceId];
  intentStoryModel.vertexByStateId[targetId] = vertexIndex[targetId];
}

void completeModelFromScenario(
    const Scenario& scenario, const DictionaryModel& dictionaryModel,
    IntentStoryModel& intentStoryModel, IntentModel& intentModel,
    ChatbotActionModel& chatbotActionModel, int& repliesCounter,
    int& vertexCounter,
    std::unordered_map<std::string, IntentStoryModel::StoryGraph::Vertex>&
        vertexIndex,
    InterpreterFeedback& interpreterFeedback) {
  // link inquiries to replies which naturally represents an edge
  std::map<int, int> inquiryToReplies;
  indexScenario(scenario, inquiryToReplies);

  // Add all intents of the scenario to the IntentModel
  IntentInserter intentInserter(dictionaryModel, scenario, intentModel);
  std::for_each(inquiryToReplies.begin(), inquiryToReplies.end(),
                intentInserter);

  // Transform inquiry to reply into an edgeDefinition
  std::vector<EdgeDefinition> edgesToInsert;
  std::unique_ptr<std::string> previousStateInScenario;
  EdgeParser edgeParser(dictionaryModel, vertexCounter, interpreterFeedback);
  EdgeParserWrapper edgeParserWrapper(scenario, previousStateInScenario,
                                      edgeParser);
  std::transform(inquiryToReplies.begin(), inquiryToReplies.end(),
                 std::back_inserter(edgesToInsert), edgeParserWrapper);

  // Retrieve fallback replies on the edges
  previousStateInScenario.reset(NULL);
  InterpreterFeedback dummyFeedback;
  EdgeParser fallbackEdgeParser(dictionaryModel, vertexCounter, dummyFeedback);
  FallbackEdgesRetriever fallbackEdgesRetriever(
      scenario, previousStateInScenario, fallbackEdgeParser, edgesToInsert);
  std::for_each(inquiryToReplies.begin(), inquiryToReplies.end(),
                fallbackEdgesRetriever);

  // adapt the replies format to chatbot model format
  ActionInserter actionInserter(chatbotActionModel, repliesCounter);
  std::for_each(edgesToInsert.begin(), edgesToInsert.end(), actionInserter);

  // Add all calculated edges to the graph
  std::for_each(edgesToInsert.begin(), edgesToInsert.end(),
                [&intentStoryModel, &vertexIndex](const EdgeDefinition& edge) {
                  addEdgeDefinitionToModel(edge, intentStoryModel, vertexIndex);
                });
}

ChatbotModel Interpreter::build(const std::string& script,
                                DictionaryModel::SharedPtr dictionaryModel,
                                InterpreterFeedback& interpreterFeedback) {
  ChatbotModel chatbotModel;

  chatbotModel.intentStoryServiceModel.intentServiceModel.dictionaryModel =
      dictionaryModel;
  chatbotModel.chatbotActionModel.reset(new ChatbotActionModel());
  chatbotModel.intentStoryServiceModel.intentStoryModel.reset(
      new IntentStoryModel());
  chatbotModel.intentStoryServiceModel.intentServiceModel.intentModel.reset(
      new IntentModel());

  ChatbotActionModel& chatbotActionModel = *chatbotModel.chatbotActionModel;
  IntentStoryModel& intentStoryModel =
      *chatbotModel.intentStoryServiceModel.intentStoryModel;
  IntentModel& intentModel =
      *chatbotModel.intentStoryServiceModel.intentServiceModel.intentModel;

  int repliesCounter = 0;
  int vertexCounter = 0;
  std::unordered_map<std::string, IntentStoryModel::StoryGraph::Vertex>
      vertexIndex;

  Scenarios scenarios;
  extractScenarios(script, scenarios);

  assert(scenarios.size() > 0);
  Scenario firstScenario = scenarios[0];

  // Change or document this constraint: root state is first state of first
  // scenario, and terminal state last state from first scenario
  assert(firstScenario.size() > 0);
  intentStoryModel.rootStateId = firstScenario[0].content;
  if (!isLine<STATE>(firstScenario[0].content))
    interpreterFeedback.push_back(
        InterpreterMessage(ROOT_STATE_MSG, firstScenario[0], ERROR));

  const DictionaryModel& dict = *dictionaryModel;

  intentStoryModel.terminalStateIds.insert(
      firstScenario[firstScenario.size() - 1].content);
  if (!isLine<STATE>(firstScenario[firstScenario.size() - 1].content))
    interpreterFeedback.push_back(InterpreterMessage(
        TERMINAL_STATE_MSG, firstScenario[firstScenario.size() - 1], ERROR));

  std::for_each(scenarios.begin(), scenarios.end(),
                [&intentStoryModel, &intentModel, &chatbotActionModel,
                 &repliesCounter, &vertexCounter, &dict, &vertexIndex,
                 &interpreterFeedback](const Scenario& scenario) {
                  completeModelFromScenario(scenario, dict, intentStoryModel,
                                            intentModel, chatbotActionModel,
                                            repliesCounter, vertexCounter,
                                            vertexIndex, interpreterFeedback);
                });

  return chatbotModel;
}
}
