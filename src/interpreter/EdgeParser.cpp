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
#include "intent/interpreter/EdgeParser.hpp"
#include "intent/interpreter/SentenceToIntentTranslator.hpp"
#define ANY_INTENT_TOKEN "_"

namespace intent {

namespace {

typedef IntentModel::IndexType IndexType;
typedef IntentModel::Intent Intent;

static const std::string ANONYMOUS_STATE = "@anonymous_state";

template <typename T>
bool checkBoundaries(const int index, const std::vector<T>& vec) {
  return index >= 0 && ((size_t)index) < vec.size();
}

std::string extractSentence(const ScriptLine& scriptLine) {
  std::string content = scriptLine.content;
  assert(content.size() > 1);
  return content.substr(1, content.size() - 1);
}

struct ParsingContext {
  ParsingContext(int& vertexCount, const DictionaryModel& dictionaryModel,
                 std::unique_ptr<std::string>& previousState)
      : vertexCount(vertexCount),
        dictionaryModel(dictionaryModel),
        previousState(previousState) {}

  int& vertexCount;
  const DictionaryModel& dictionaryModel;
  std::unique_ptr<std::string>& previousState;
};

void completeSourceState(const Scenario& scenario,
                         const std::pair<int, int>& inquiryToReply,
                         ParsingContext& context, EdgeDefinition& edge,
                         InterpreterFeedback& interpreterFeedback) {
  int potentialSourceIdIndex = inquiryToReply.first - 1;

  // Get source state
  if (context.previousState.get()) {
    edge.source.stateId = *context.previousState;
  } else if (checkBoundaries(potentialSourceIdIndex, scenario) &&
             isLine<STATE>(scenario[potentialSourceIdIndex])) {
    edge.source.stateId = scenario[potentialSourceIdIndex].content;
  } else {
    edge.source.stateId = ANONYMOUS_STATE + std::to_string(context.vertexCount);
    interpreterFeedback.push_back(InterpreterMessage(
        ANONYMOUS_STATE_CREATION, scenario[inquiryToReply.first]));
    context.vertexCount++;
  }
}

void completeTargetState(const Scenario& scenario,
                         const std::pair<int, int>& inquiryToReply,
                         ParsingContext& context, EdgeDefinition& edge,
                         InterpreterFeedback& interpreterFeedback) {
  int potentialTargetIdIndex = inquiryToReply.second + 1;

  // Get target state
  if (checkBoundaries(potentialTargetIdIndex, scenario) &&
      isLine<STATE>(scenario[potentialTargetIdIndex])) {
    edge.target.stateId = scenario[potentialTargetIdIndex].content;
  } else {
    edge.target.stateId = ANONYMOUS_STATE + std::to_string(context.vertexCount);
    interpreterFeedback.push_back(InterpreterMessage(
        ANONYMOUS_STATE_CREATION, scenario[inquiryToReply.second]));
    context.vertexCount++;
  }
  context.previousState.reset(new std::string(edge.target.stateId));
}

void completeEdgeInfo(const Scenario& scenario,
                      const std::pair<int, int>& inquiryToReply,
                      ParsingContext& context, EdgeDefinition& edge,
                      InterpreterFeedback& interpreterFeedback) {
  int potentialActionIdIndex = inquiryToReply.first + 1;
  std::string inquiry = extractSentence(scenario[inquiryToReply.first]);

  // Get action line
  if (checkBoundaries(potentialActionIdIndex, scenario) &&
      isLine<ACTION>(scenario[potentialActionIdIndex])) {
    edge.edge.actionId = scenario[potentialActionIdIndex].content;
  } else {
    interpreterFeedback.push_back(InterpreterMessage(
        NO_ACTION, scenario[potentialActionIdIndex], WARNING));
  }

  // identify the intent to complete intentModel and edge
  std::pair<IndexType, Intent> intent =
      SentenceToIntentTranslator::translate(inquiry, context.dictionaryModel);
  edge.edge.intentId = intent.first;

  if (intent.first.empty()) {
    interpreterFeedback.push_back(
        InterpreterMessage(NO_ENTITY, scenario[inquiryToReply.first], WARNING));
  }
}

}  // anonymous

EdgeDefinition EdgeParser::parse(
    const Scenario& scenario, const std::pair<int, int>& inquiryToReply,
    std::unique_ptr<std::string>& previousStateInScenario) {
  EdgeDefinition edge;
  ParsingContext context(m_vertexCounter, m_dictionaryModel,
                         previousStateInScenario);

  completeSourceState(scenario, inquiryToReply, context, edge,
                      m_interpreterFeedback);

  completeTargetState(scenario, inquiryToReply, context, edge,
                      m_interpreterFeedback);

  completeEdgeInfo(scenario, inquiryToReply, context, edge,
                   m_interpreterFeedback);

  if (checkBoundaries(inquiryToReply.second, scenario))
    edge.replyTemplate = extractSentence(scenario[inquiryToReply.second]);

  return edge;
}

std::unique_ptr<EdgeDefinition> EdgeParser::parseFallback(
    const Scenario& scenario, const std::pair<int, int>& inquiryToReply,
    std::unique_ptr<std::string>& previousStateInScenario) {
  EdgeDefinition edge =
      parse(scenario, inquiryToReply, previousStateInScenario);
  std::unique_ptr<EdgeDefinition> fallbackEdge;

  int potentialFallbackReplyIdIndex = inquiryToReply.first + 2;
  if (checkBoundaries(potentialFallbackReplyIdIndex, scenario) &&
      isLine<FALLBACK>(scenario[potentialFallbackReplyIdIndex])) {
    fallbackEdge.reset(new EdgeDefinition(edge));
    fallbackEdge->replyTemplate =
        extractSentence(scenario[potentialFallbackReplyIdIndex]);
    fallbackEdge->target = fallbackEdge->source;
    fallbackEdge->edge.actionId = fallbackEdge->source.stateId + "*";
    fallbackEdge->edge.intentId = ANY_INTENT_TOKEN;
  }
  return fallbackEdge;
}
}
