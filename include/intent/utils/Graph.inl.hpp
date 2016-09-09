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
// Created by clement on 07/05/16.
//

#include <iostream>
#include "boost/graph/graphviz.hpp"

namespace intent {
template <typename VertexInfo, typename EdgeInfo>
Graph<VertexInfo, EdgeInfo>::Graph() {}

template <typename VertexInfo, typename EdgeInfo>
typename Graph<VertexInfo, EdgeInfo>::Vertex
Graph<VertexInfo, EdgeInfo>::addVertex(const VertexInfo& vertexInfo) {
  _Vertex v = boost::add_vertex(m_graph);
  m_graph[v] = vertexInfo;

  return Vertex(m_graph, v);
}

template <typename VertexInfo, typename EdgeInfo>
typename Graph<VertexInfo, EdgeInfo>::Edge Graph<VertexInfo, EdgeInfo>::addEdge(
    const Vertex& source, const Vertex& target, const EdgeInfo& edgeInfo) {
  std::pair<_Edge, bool> p =
      boost::add_edge(source.getVertex(), target.getVertex(), m_graph);
  m_graph[p.first] = edgeInfo;

  return Edge(m_graph, p.first);
}

template <typename VertexInfo, typename EdgeInfo>
typename Graph<VertexInfo, EdgeInfo>::Edges
Graph<VertexInfo, EdgeInfo>::nextEdges(
    const typename Graph<VertexInfo, EdgeInfo>::Vertex& v) const {
  typedef typename Graph<VertexInfo, EdgeInfo>::Edges Edges;
  typedef typename Graph<VertexInfo, EdgeInfo>::_Graph _Graph;

  Edges outEdgesFromCurrentNode;
  typename boost::graph_traits<_Graph>::out_edge_iterator e, e_end;
  for (boost::tie(e, e_end) = boost::out_edges(v.getVertex(), m_graph);
       e != e_end; ++e) {
    Edge edge(m_graph, *e);
    outEdgesFromCurrentNode.push_back(edge);
  }
  return outEdgesFromCurrentNode;
}

template <typename VertexInfo, typename EdgeInfo>
template <typename WriterMaker>
void typename Graph<VertexInfo, EdgeInfo>::dump(std::ostream& ostream,
                                                WriterMaker& wm) const {
  boost::write_graphviz(ostream, m_graph, wm.makeVertexWriter(m_graph),
                        wm.makeEdgeWriter(m_graph));
};
}
