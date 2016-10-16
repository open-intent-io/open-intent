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
#ifndef INTENT_INTENTSTORYMODEL_HPP
#define INTENT_INTENTSTORYMODEL_HPP

#include "intent/intent_service/IntentModel.hpp"

#include "intent/utils/Graph.hpp"

#include <unordered_set>
#include <memory>

namespace intent {
/**
 * \brief A data model for intent story. It represents a graph with states and
 * user intents on the edges.
 */
class IntentStoryModel {
 public:
  typedef IntentModel::IndexType IndexType;
  typedef IntentModel::Intent Intent;

  /**
   * \brief The information stored in the edges of the graph.
   */
  struct EdgeInfo {
    IntentStoryModel::Intent intent;
    std::string actionId;
    std::string reply;
  };

  /**
   * \brief The information stored in the vertices of the graph.
   */
  struct VertexInfo {
    std::string stateId;
  };

  typedef Graph<VertexInfo, EdgeInfo> StoryGraph;
  typedef std::map<IndexType, StoryGraph::Vertex> VertexByStateIdIndex;
  typedef std::unordered_set<IndexType> StateIdSet;
  typedef std::shared_ptr<IntentStoryModel> SharedPtr;

  /**
   * \brief The root state ID.
   */
  IndexType rootStateId;

  /**
   * \brief Index of the vertices by state ID
   */
  VertexByStateIdIndex vertexByStateId;

  /**
   * \brief The directed graph.
   */
  StoryGraph graph;
};
}

#endif  // INTENT_INTENTSTORYMODEL_HPP
