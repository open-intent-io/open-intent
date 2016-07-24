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
#ifndef INTENT_GRAPH_HPP
#define INTENT_GRAPH_HPP

#include "boost/graph/adjacency_list.hpp"
#include <boost/graph/directed_graph.hpp>

#include "intent/utils/Exception.hpp"

namespace intent {
/**
 * \brief Representation of a graph using adjacency list with additional
 * information on the vertices and edges.
 */
template <class VertexInfo, class EdgeInfo>
class Graph {
 public:
  typedef
      typename boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                                     VertexInfo, EdgeInfo> _Graph;

  using _Vertex = typename _Graph::vertex_descriptor;
  using _Edge = typename _Graph::edge_descriptor;

 public:
  /**
   * \brief Representation of a vertex from which information can be extracted.
   */
  class Vertex {
   public:
    Vertex() : m_graph(NULL), m_vertex() {}

    Vertex(const _Graph& graph, const _Vertex& vertex)
        : m_graph(&graph), m_vertex(vertex) {}

    Vertex(const Vertex& that)
        : m_graph(that.m_graph), m_vertex(that.m_vertex) {}

    inline _Vertex getVertex() const { return m_vertex; }

    inline const VertexInfo& getInfo() const { return (*m_graph)[m_vertex]; }

   private:
    const _Graph* m_graph;
    _Vertex m_vertex;
  };

  /**
   * \brief Representation of an edge from which information can be extracted.
   */
  class Edge {
   public:
    Edge() : m_graph(NULL), m_edge() {}

    Edge(const _Graph& graph, const _Edge& edge)
        : m_graph(&graph), m_edge(edge) {}

    Edge(const Edge& that) : m_graph(that.m_graph), m_edge(that.m_edge) {}

    inline _Edge getEdge() const { return m_edge; }

    inline const EdgeInfo& getInfo() const { return (*m_graph)[m_edge]; }

    inline Vertex getSource() const {
      return Vertex(*m_graph, boost::source(m_edge, *m_graph));
    }

    inline Vertex getTarget() const {
      return Vertex(*m_graph, boost::target(m_edge, *m_graph));
    }

   private:
    const _Graph* m_graph;
    _Edge m_edge;
  };

  using Vertices = std::vector<Vertex>;
  using Edges = std::vector<Edge>;

  Graph();

  /**
   * \brief Add a vertex to the graph with some information attached to it.
   * \param vertexInfo    The information to attach to the vertex
   * \return The vertex.
   */
  Vertex addVertex(const VertexInfo& vertexInfo);

  /**
   * \brief Add an edge to the graph with some information attached to it.
   * \param source    The source vertex. (this is a directed graph).
   * \param target    The target vertex. (this is a directed graph).
   * \return  The edge.
   */
  Edge addEdge(const Vertex& source, const Vertex& target,
               const EdgeInfo& edgeInfo);

  /**
   * \brief Returns a list of neighboor edges.
   * \param v     The vertex from which to get the neighboors.
   * \return The neighboor edges.
   */
  Edges nextEdges(const Vertex& v) const;

  /**
   * \brief Returns the number of vertices in the graph.
   * \return  The number of vertices.
   */
  std::size_t vertexCount() const { return boost::num_vertices(m_graph); }

  /**
   * \brief Returns the number of edges in the graph.
   * \return The number of edges.
   */
  std::size_t edgeCount() const { return boost::num_edges(m_graph); }

  /**
   * \brief Dumps the graph
   * \param ostream   The stream to dump the graph to.
   * \param vertexWriterMaker The vertex writer factory that creates the vertex
   * serializer.
   */
  template <typename VertexWriterMaker>
  void dump(std::ostream& ostream, VertexWriterMaker& vertexWriterMaker) const;

 private:
  _Graph m_graph;
};

/**
 * \brief Any exception raised by an operation on a graph.
 */
class GraphException : public Exception {
 public:
  GraphException() {}
};
}

#include "Graph.inl.hpp"

#endif
