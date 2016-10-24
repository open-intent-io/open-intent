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
#include "intent/intent_story_service/IntentStoryModelSerializer.hpp"

#include <boost/algorithm/string/join.hpp>

namespace intent {
typedef boost::property_map<IntentStoryModel::StoryGraph::_Graph,
                            std::string IntentStoryModel::VertexInfo::
                                *>::const_type VertexStringPropertyMap;
typedef boost::property_map<IntentStoryModel::StoryGraph::_Graph,
                            std::string IntentStoryModel::EdgeInfo::
                                *>::const_type EdgeStringPropertyMap;
typedef boost::property_map<
    IntentStoryModel::StoryGraph::_Graph,
    IntentStoryModel::Intent IntentStoryModel::EdgeInfo::*>::const_type
    EdgeIntentPropertyMap;

struct VertexWriter {
  VertexWriter(VertexStringPropertyMap stateId,
               const IntentStoryServiceModel &intentStoryServiceModel)
      : m_stateId(stateId),
        m_intentStoryServiceModel(intentStoryServiceModel) {}

  template <class Vertex>
  void operator()(std::ostream &out, const Vertex &v) const {
    const std::string &actionId = m_stateId[v];

    out << "[label=<" << actionId << ">"
        << "]";
  }

  VertexStringPropertyMap m_stateId;
  const IntentStoryServiceModel &m_intentStoryServiceModel;
};

std::string entitiesOfIntent(
    const std::string intentId,
    const IntentStoryServiceModel &intentStoryServiceModel) {
  std::stringstream entities;
  IntentModel::IntentIndex::const_iterator it =
      intentStoryServiceModel.intentServiceModel.intentModel->intentsByIntentId
          .find(intentId);
  if (it !=
      intentStoryServiceModel.intentServiceModel.intentModel->intentsByIntentId
          .end()) {
    IntentModel::Intent intent = it->second;
    std::vector<std::string> entityNames;
    std::transform(
        intent.entityToVariableNames.begin(),
        intent.entityToVariableNames.end(), std::back_inserter(entityNames),
        [](const std::pair<std::string, std::queue<std::string> > &pair) {
          return pair.first;
        });
    entities << "[" << boost::algorithm::join(entityNames, ", ") << "]";
  }
  return entities.str();
}

struct EdgeWriter {
  EdgeWriter(EdgeIntentPropertyMap intent, EdgeStringPropertyMap actionId,
             EdgeStringPropertyMap reply,
             const IntentStoryServiceModel &intentStoryServiceModel)
      : m_intent(intent),
        m_actionId(actionId),
        m_reply(reply),
        m_intentStoryServiceModel(intentStoryServiceModel) {}

  template <class Edge>
  void operator()(std::ostream &out, const Edge &e) const {
    const IntentStoryModel::Intent intent = m_intent[e];
    const std::string &actionId = m_actionId[e];
    const std::string &reply = m_reply[e];

    out << "[label=< <table BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
        << "<tr>"
        << "<td>intent_id</td><td>" << intent.intentId << "</td></tr>"
        << "<tr>"
        << "<td>intent_desc</td><td>" << intent.example << "</td></tr>"
        << "<tr>"
        << "<td>entities</td><td>"
        << entitiesOfIntent(intent.intentId, m_intentStoryServiceModel)
        << "</td></tr>"
        << "<tr>"
        << "<td>action</td><td>" << actionId << "</td></tr>"
        << "<tr>"
        << "<td>reply</td><td>" << reply << "</td></tr>"
        << "</table> >]";
  }

  EdgeIntentPropertyMap m_intent;
  EdgeStringPropertyMap m_actionId;
  EdgeStringPropertyMap m_reply;
  const IntentStoryServiceModel &m_intentStoryServiceModel;
};

struct WriterMaker {
  WriterMaker(const IntentStoryServiceModel &intentStoryServiceModel)
      : m_intentStoryServiceModel(intentStoryServiceModel) {}

  VertexWriter makeVertexWriter(
      const IntentStoryModel::StoryGraph::_Graph &graph) const {
    return VertexWriter(
        boost::get(&IntentStoryModel::VertexInfo::stateId, graph),
        m_intentStoryServiceModel);
  }

  EdgeWriter makeEdgeWriter(
      const IntentStoryModel::StoryGraph::_Graph &graph) const {
    return EdgeWriter(boost::get(&IntentStoryModel::EdgeInfo::intent, graph),
                      boost::get(&IntentStoryModel::EdgeInfo::actionId, graph),
                      boost::get(&IntentStoryModel::EdgeInfo::reply, graph),
                      m_intentStoryServiceModel);
  }

  const IntentStoryServiceModel &m_intentStoryServiceModel;
};

void IntentStoryModelSerializer::serialize(
    std::ostream &stream,
    const IntentStoryServiceModel &intentStoryServiceModel) const {
  WriterMaker writerMaker(intentStoryServiceModel);
  intentStoryServiceModel.intentStoryModel->graph.dump(stream, writerMaker);
}
}
