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
//
// Created by clement on 04/05/16.
//

#include <intent/chatbot/ChatbotModel.hpp>
#include "intent/utils/Deserializer.hpp"
#include "intent/utils/SingleCharacterDelimiterTokenizer.hpp"
#include "intent/intent_service/IntentEncoder.hpp"
#include "json.hpp"

namespace intent {
const std::string Deserializer::REGEX_SYMBOL = "regex";

bool isTermRegex(const nlohmann::json::object_t::const_iterator& term) {
  const std::string termText = term->first;
  return termText == Deserializer::REGEX_SYMBOL;
}

void deserializeDictionary(const nlohmann::json::object_t& dictionary,
                           DictionaryModel& dictionaryModel) {
  nlohmann::json::object_t::const_iterator entitiesIt = dictionary.begin();
  nlohmann::json::object_t::const_iterator entitiesItEnd = dictionary.end();

  unsigned int termId = 0;
  for (unsigned int i = 0; entitiesIt != entitiesItEnd; ++entitiesIt, i++) {
    dictionaryModel.entitiesByEntityId[i] = entitiesIt->first;

    const nlohmann::json::object_t& termsAndAlias = entitiesIt->second;
    nlohmann::json::object_t::const_iterator termsAndAliasIt =
        termsAndAlias.begin();
    nlohmann::json::object_t::const_iterator termsAndAliasItEnd =
        termsAndAlias.end();

    for (; termsAndAliasIt != termsAndAliasItEnd; ++termsAndAliasIt) {
      if (isTermRegex(termsAndAliasIt)) {
        dictionaryModel.regexesByEntityId[termsAndAliasIt->second] = i;
      } else {
        Term term;

        term.term = termsAndAliasIt->first;
        term.termId = termId;
        term.entityId = i;

        nlohmann::json::array_t alias = termsAndAliasIt->second;
        nlohmann::json::array_t::const_iterator aliasIt = alias.begin();
        nlohmann::json::array_t::const_iterator aliasItEnd = alias.end();

        for (unsigned int k = 0; aliasIt != aliasItEnd; aliasIt++, ++k) {
          term.alias.push_back(*aliasIt);
        }

        dictionaryModel.dictionary.pushTerm(term);
      }
      termId++;
    }
  }
}

int findEntity(const std::unordered_map<int, std::string>& entities,
               const std::string& value) {
  for (std::unordered_map<int, std::string>::const_iterator it =
           entities.begin();
       it != entities.end(); ++it) {
    if (it->second == value) {
      return it->first;
    }
  }
  return -1;
}

void deserializeIntentSchema(const nlohmann::json::array_t& intentVector,
                             const DictionaryModel& dictionaryModel,
                             IntentModel::Intent& intent) {
  std::unordered_map<std::string, int> entitiesCounter;
  for (unsigned int i = 0; i < intentVector.size(); ++i) {
    const std::string& namedEntity = intentVector.at(i);
    std::vector<std::string> entityTokens;
    SingleCharacterDelimiterTokenizer::tokenize(namedEntity, ":", entityTokens);

    std::string entity;
    std::string name;
    if (entityTokens.size() == 2) {
      entity = entityTokens[0];
      name = entityTokens[1];
    } else if (entityTokens.size() == 1) {
      entity = entityTokens[0];
      name = entity + std::to_string(entitiesCounter[entity]);
      ++entitiesCounter[entity];
    }

    if (!entity.empty()) {
      int entityId = findEntity(dictionaryModel.entitiesByEntityId, entity);
      intent.entities.push_back(entityId);

      // TODO : check if name already in queue
      intent.entityToVariableNames[entity].push(name);
    }
  }
}

void deserializeIntents(const nlohmann::json::array_t& intents,
                        const DictionaryModel& dictionaryModel,
                        IntentModel& intentModel) {
  nlohmann::json::array_t::const_iterator intentIt = intents.begin();
  nlohmann::json::array_t::const_iterator intentItEnd = intents.end();

  for (; intentIt != intentItEnd; ++intentIt) {
    const nlohmann::json& intentJson = *intentIt;

    IntentModel::Intent intent;
    nlohmann::json::const_iterator intentIt = intentJson.find("intent");
    const std::string& intentId = intentJson["id"].get<std::string>();

    if (intentIt != intentJson.end()) {
      deserializeIntentSchema(*intentIt, dictionaryModel, intent);
    }

    nlohmann::json::const_iterator exampleIt = intentJson.find("example");
    if (exampleIt != intentJson.end()) {
      intent.example = *exampleIt;
    }

    intentModel.intentsByIntentId[intentId] = intent;
  }
}

void deserializeGraph(const nlohmann::json::object_t& graph,
                      IntentStoryModel& intentStoryModel) {
  nlohmann::json::object_t::const_iterator sourceEdgeIt = graph.begin();
  nlohmann::json::object_t::const_iterator sourceEdgeItEnd = graph.end();

  for (; sourceEdgeIt != sourceEdgeItEnd; ++sourceEdgeIt) {
    const std::string& sourceId = sourceEdgeIt->first;
    const nlohmann::json::object_t& edgeTarget = sourceEdgeIt->second;

    nlohmann::json::object_t::const_iterator edgeTargetIt = edgeTarget.begin();
    nlohmann::json::object_t::const_iterator edgeTargetItEnd = edgeTarget.end();

    for (; edgeTargetIt != edgeTargetItEnd; ++edgeTargetIt) {
      const std::string& intentAndAction = edgeTargetIt->first;
      std::vector<std::string> tokens;
      SingleCharacterDelimiterTokenizer::tokenize(intentAndAction, ":", tokens);

      std::string intentName;
      std::string actionId;
      intentName = tokens[0];
      if (tokens.size() == 2) actionId = tokens[1];

      const std::string& targetId = edgeTargetIt->second;

      IntentStoryModel::EdgeInfo edgeInfo;
      edgeInfo.intentId = intentName;
      edgeInfo.actionId = actionId;
      edgeInfo.label = intentAndAction;

      IntentStoryModel::StoryGraph::Vertex source;
      IntentStoryModel::StoryGraph::Vertex target;

      IntentStoryModel::VertexByStateIdIndex::const_iterator it =
          intentStoryModel.vertexByStateId.find(sourceId);
      if (it != intentStoryModel.vertexByStateId.end()) {
        source = it->second;
      } else  // Adding vertex
      {
        IntentStoryModel::VertexInfo vertexInfo;
        vertexInfo.stateId = sourceId;
        source = intentStoryModel.graph.addVertex(vertexInfo);
        intentStoryModel.vertexByStateId[sourceId] = source;
      }

      it = intentStoryModel.vertexByStateId.find(targetId);
      if (it != intentStoryModel.vertexByStateId.end()) {
        target = it->second;
      } else  // Adding vertex
      {
        IntentStoryModel::VertexInfo vertexInfo;
        vertexInfo.stateId = targetId;
        target = intentStoryModel.graph.addVertex(vertexInfo);
        intentStoryModel.vertexByStateId[targetId] = target;
      }

      intentStoryModel.graph.addEdge(source, target, edgeInfo);
    }
  }
}

void deserializeIntentStory(const nlohmann::json& story,
                            IntentStoryModel& intentStoryModel) {
  nlohmann::json::const_iterator it;

  it = story.find("root");
  if (it != story.end() && (*it).is_string()) {
    IntentStoryModel::VertexInfo vInfo;
    vInfo.stateId = *it;
    IntentStoryModel::StoryGraph::Vertex v =
        intentStoryModel.graph.addVertex(vInfo);
    intentStoryModel.vertexByStateId[vInfo.stateId] = v;
    intentStoryModel.rootStateId = vInfo.stateId;
  }

  it = story.find("terminals");
  if (it != story.end() && (*it).is_array()) {
    const nlohmann::json::array_t& terminals = *it;
    for (const std::string& terminalStateId : terminals) {
      IntentStoryModel::VertexInfo vInfo;
      vInfo.stateId = terminalStateId;
      IntentStoryModel::StoryGraph::Vertex v =
          intentStoryModel.graph.addVertex(vInfo);
      intentStoryModel.vertexByStateId[vInfo.stateId] = v;
      intentStoryModel.terminalStateIds.insert(terminalStateId);
    }
  }

  it = story.find("graph");
  if (it != story.end() && (*it).is_object()) {
    deserializeGraph(*it, intentStoryModel);
  }
}

void deserializeIntentModel(const nlohmann::json& input,
                            const DictionaryModel& dictionaryModel,
                            IntentModel& intentModel) {
  nlohmann::json::const_iterator it = input.find("intents");
  if (it != input.end() && (*it).is_array()) {
    const nlohmann::json::array_t& intents = *it;
    deserializeIntents(intents, dictionaryModel, intentModel);
  }
}

void deserializeDictionaryModel(const nlohmann::json& input,
                                DictionaryModel& dictionaryModel) {
  nlohmann::json::const_iterator it = input.find("entities");
  if (it != input.end() && (*it).is_object()) {
    deserializeDictionary(*it, dictionaryModel);
  }
}

DictionaryModel::SharedPtr Deserializer::deserialize(
    std::istream& is, Type2Type<DictionaryModel::SharedPtr> type) {
  std::istreambuf_iterator<char> eos;
  std::string s(std::istreambuf_iterator<char>(is), eos);
  nlohmann::json document;
  try {
    document = nlohmann::json::parse(s);
  } catch (...) {
    throw DeserializerException("Error while parsing JSON");
  }
  return deserialize(document, type);
}

DictionaryModel::SharedPtr Deserializer::deserialize(
    const nlohmann::json& input, Type2Type<DictionaryModel::SharedPtr>) {
  DictionaryModel::SharedPtr dictionaryModel(new DictionaryModel());

  try {
    deserializeDictionaryModel(input, *dictionaryModel);
  } catch (...) {
    throw DeserializerException("Error while parsing JSON");
  }

  return dictionaryModel;
}

IntentServiceModel Deserializer::deserialize(
    std::istream& is, Type2Type<IntentServiceModel> type) {
  std::istreambuf_iterator<char> eos;
  std::string s(std::istreambuf_iterator<char>(is), eos);

  nlohmann::json document;
  try {
    document = nlohmann::json::parse(s);
  } catch (...) {
    throw DeserializerException("Error while parsing JSON");
  }

  return deserialize(document, type);
}

IntentServiceModel Deserializer::deserialize(
    const nlohmann::json& input, Type2Type<IntentServiceModel> type) {
  IntentServiceModel intentServiceModel;
  intentServiceModel.dictionaryModel.reset(new DictionaryModel());
  intentServiceModel.intentModel.reset(new IntentModel());

  try {
    deserializeDictionaryModel(input, *intentServiceModel.dictionaryModel);
    deserializeIntentModel(input, *intentServiceModel.dictionaryModel,
                           *intentServiceModel.intentModel);
  } catch (...) {
    throw DeserializerException("Error while parsing JSON");
  }

  return intentServiceModel;
}

IntentStoryServiceModel Deserializer::deserialize(
    std::istream& is, Type2Type<IntentStoryServiceModel> type) {
  std::istreambuf_iterator<char> eos;
  std::string s(std::istreambuf_iterator<char>(is), eos);

  nlohmann::json document;
  try {
    document = nlohmann::json::parse(s);
  } catch (...) {
    throw DeserializerException("Error while parsing JSON");
  }

  return deserialize(document, type);
}

void deserializeIntentStoryModel(const nlohmann::json& input,
                                 IntentStoryModel& intentStoryModel) {
  nlohmann::json::const_iterator it;

  it = input.find("intent_story");
  if (it != input.end() && (*it).is_object()) {
    deserializeIntentStory(*it, intentStoryModel);
  }
}

IntentStoryServiceModel Deserializer::deserialize(
    const nlohmann::json& input, Type2Type<IntentStoryServiceModel> type) {
  IntentStoryServiceModel intentStoryServiceModel;

  intentStoryServiceModel.intentStoryModel =
      IntentStoryModel::SharedPtr(new IntentStoryModel());
  intentStoryServiceModel.intentServiceModel.intentModel =
      IntentModel::SharedPtr(new IntentModel());
  intentStoryServiceModel.intentServiceModel.dictionaryModel =
      DictionaryModel::SharedPtr(new DictionaryModel());

  try {
    deserializeDictionaryModel(
        input, *intentStoryServiceModel.intentServiceModel.dictionaryModel);
    deserializeIntentModel(
        input, *intentStoryServiceModel.intentServiceModel.dictionaryModel,
        *intentStoryServiceModel.intentServiceModel.intentModel);
    deserializeIntentStoryModel(input,
                                *intentStoryServiceModel.intentStoryModel);
  } catch (...) {
    throw DeserializerException("Error while parsing JSON");
  }

  return intentStoryServiceModel;
}

void deserializeReplies(const nlohmann::json::object_t& replies,
                        ChatbotActionModel& chatbotActionModel) {
  nlohmann::json::object_t::const_iterator replyIt = replies.begin();
  nlohmann::json::object_t::const_iterator replyItEnd = replies.end();

  for (; replyIt != replyItEnd; ++replyIt) {
    const std::string& replyId = replyIt->first;
    const std::string& reply = replyIt->second;

    chatbotActionModel.replyContentByReplyIdIndex[replyId] = reply;
  }
}

void deserializeReplyIdsByActionId(const nlohmann::json::object_t& actions,
                                   ChatbotActionModel& chatbotActionModel) {
  nlohmann::json::object_t::const_iterator actionIt = actions.begin();
  nlohmann::json::object_t::const_iterator actionItEnd = actions.end();

  for (; actionIt != actionItEnd; ++actionIt) {
    const std::string& actionId = actionIt->first;
    const nlohmann::json::array_t& repliesList = actionIt->second;

    for (const std::string& replyId : repliesList) {
      chatbotActionModel.replyIdsByActionId[actionId].push_back(replyId);
    }
  }
}

void deserializeChatbotActions(const nlohmann::json& chatbot,
                               ChatbotActionModel& chatbotActionModel) {
  nlohmann::json::const_iterator it;

  it = chatbot.find("replies");
  if (it != chatbot.end() && (*it).is_object()) {
    deserializeReplies(*it, chatbotActionModel);
  }

  it = chatbot.find("replies_by_action");
  if (it != chatbot.end() && (*it).is_object()) {
    deserializeReplyIdsByActionId(*it, chatbotActionModel);
  }
}

void deserializeChatbotActionModel(const nlohmann::json& input,
                                   ChatbotActionModel& chatbotActionModel) {
  nlohmann::json::const_iterator it;

  it = input.find("chatbot");
  if (it != input.end() && (*it).is_object()) {
    deserializeChatbotActions(*it, chatbotActionModel);
  }
}

ChatbotModel Deserializer::deserialize(std::istream& is,
                                       Type2Type<ChatbotModel> type) {
  std::istreambuf_iterator<char> eos;
  std::string s(std::istreambuf_iterator<char>(is), eos);

  nlohmann::json document;
  try {
    document = nlohmann::json::parse(s);
  } catch (...) {
    throw DeserializerException("Error while parsing JSON");
  }

  return deserialize(document, type);
}

ChatbotModel Deserializer::deserialize(const nlohmann::json& input,
                                       Type2Type<ChatbotModel> type) {
  ChatbotModel chatbotModel;
  chatbotModel.chatbotActionModel =
      ChatbotActionModel::SharedPtr(new ChatbotActionModel());
  chatbotModel.intentStoryServiceModel.intentStoryModel =
      IntentStoryModel::SharedPtr(new IntentStoryModel());
  chatbotModel.intentStoryServiceModel.intentServiceModel.intentModel =
      IntentModel::SharedPtr(new IntentModel());
  chatbotModel.intentStoryServiceModel.intentServiceModel.dictionaryModel =
      DictionaryModel::SharedPtr(new DictionaryModel());

  try {
    deserializeDictionaryModel(input, *chatbotModel.intentStoryServiceModel
                                           .intentServiceModel.dictionaryModel);
    deserializeIntentModel(
        input, *chatbotModel.intentStoryServiceModel.intentServiceModel
                    .dictionaryModel,
        *chatbotModel.intentStoryServiceModel.intentServiceModel.intentModel);
    deserializeIntentStoryModel(
        input, *chatbotModel.intentStoryServiceModel.intentStoryModel);
    deserializeChatbotActionModel(input, *chatbotModel.chatbotActionModel);
  } catch (...) {
    throw DeserializerException("Error while parsing JSON");
  }

  return chatbotModel;
}
}
