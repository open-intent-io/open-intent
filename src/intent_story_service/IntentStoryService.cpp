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
// Created by clement on 11/05/16.
//

#include "intent/intent_story_service/IntentStoryService.hpp"
#include "intent/intent_service/IntentEncoder.hpp"

#include "intent/utils/Deserializer.hpp"
#include "intent/utils/Logger.hpp"
#include <fstream>

#define ANY_INTENT_TOKEN "_"

namespace intent {
IntentStoryService::IntentStoryService(
    const IntentStoryServiceModel& intentStoryServiceModel)
    : IntentService(intentStoryServiceModel.intentServiceModel),
      m_intentStoryModel(intentStoryServiceModel.intentStoryModel) {}

typedef std::unordered_map<IntentModel::IndexType,
                           IntentStoryModel::StoryGraph::Edge>
    EdgeByIntentIdIndex;

void buildIntentsIndex(
    const IntentModel& intentModel,
    const IntentStoryModel::StoryGraph::Edges edges,
    IntentModel::IntentIndex& intentByIdIndex,
    EdgeByIntentIdIndex& edgeByIntentIdIndex,
    boost::optional<IntentStoryModel::StoryGraph::Edge>& anyIntentEdge) {
  for (const IntentStoryModel::StoryGraph::Edge& e : edges) {
    const IntentModel::IndexType& intentId = e.getInfo().intent.intentId;

    IntentModel::IntentIndex::const_iterator it =
        intentModel.intentsByIntentId.find(intentId);
    if (intentId == ANY_INTENT_TOKEN) {
      anyIntentEdge = e;
    } else if (it != intentModel.intentsByIntentId.end()) {
      intentByIdIndex.insert(std::make_pair(intentId, it->second));
      edgeByIntentIdIndex.insert(std::make_pair(intentId, e));
    }
  }
}

IntentStoryService::Result IntentStoryService::evaluate(
    const std::string& stateId, const std::string& message) const {
  LOG_INFO() << "Look for intent in \"" + message + "\" from state \"" +
                    stateId + "\".";

  IntentStoryService::Result intentStoryResult;
  IntentStoryModel::VertexByStateIdIndex::const_iterator vIt =
      m_intentStoryModel->vertexByStateId.find(stateId);

  if (vIt != m_intentStoryModel->vertexByStateId.end()) {
    IntentStoryModel::StoryGraph::Edges neighboorEdges =
        m_intentStoryModel->graph.nextEdges(vIt->second);

    IntentModel::IntentIndex intentByIdIndex;
    EdgeByIntentIdIndex edgeByIntentIdIndex;
    // If no intent is matching, the user can define a fallback edge to handle
    // fallback replies
    boost::optional<IntentStoryModel::StoryGraph::Edge> otherwiseIntent;

    buildIntentsIndex(*m_intentServiceModel.intentModel, neighboorEdges,
                      intentByIdIndex, edgeByIntentIdIndex, otherwiseIntent);
    IntentMatcher::IntentResult intentResult = resolveIntent(
        message, *m_intentServiceModel.dictionaryModel, intentByIdIndex);

    EdgeByIntentIdIndex::const_iterator foundEdge =
        edgeByIntentIdIndex.find(intentResult.intent.intentId);

    if (foundEdge != edgeByIntentIdIndex.end()) {
      intentStoryResult.found = intentResult.found;
      intentStoryResult.actionId = foundEdge->second.getInfo().actionId;
      intentStoryResult.intent = intentResult.intent;
      intentStoryResult.nextStateId =
          foundEdge->second.getTarget().getInfo().stateId;
    } else if (otherwiseIntent) {
      LOG_DEBUG() << "Otherwise intent detected.";
      intentStoryResult.found = true;
      intentStoryResult.actionId = otherwiseIntent->getInfo().actionId;
      intentStoryResult.nextStateId =
          otherwiseIntent->getTarget().getInfo().stateId;
    }
  } else {
    LOG_ERROR() << "There are no neighboor edges from state \"" + stateId +
                       "\".";
  }

  LOG_TRACE() << "Result = " << intentStoryResult;

  return intentStoryResult;
}

std::ostream& operator<<(std::ostream& os,
                         const IntentStoryService::Result& result) {
  return os << "{ found: " << result.found << ", "
            << "nextStateId: " << result.nextStateId << ", "
            << "intent: " << result.intent << " }";
}
}
