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
// Created by clement on 12/05/16.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "intent/utils/Graph.hpp"

using namespace ::testing;

namespace intent
{
    namespace test
    {
        TEST(GraphTest, adding_vertices)
        {
            typedef Graph<int, int> G;

            G g;

            G::Vertex v = g.addVertex(4);

            EXPECT_EQ(0, static_cast<int>(g.edgeCount()));
            EXPECT_EQ(1, static_cast<int>(g.vertexCount()));
            EXPECT_EQ(4, v.getInfo());
        }

        TEST(GraphTest, adding_edge)
        {
            typedef Graph<int, int> G;
            G g;

            G::Vertex v1 = g.addVertex(1);
            G::Vertex v2 = g.addVertex(2);

            G::Edge e = g.addEdge(v1, v2, 3);

            EXPECT_EQ(2, static_cast<int>(g.vertexCount()));
            EXPECT_EQ(1, static_cast<int>(g.edgeCount()));

            EXPECT_EQ(1, v1.getInfo());
            EXPECT_EQ(2, v2.getInfo());
            EXPECT_EQ(3, e.getInfo());
        }

        TEST(GraphTest, return_the_neighboor_edges)
        {
            typedef Graph<int, int> G;
            G g;

            G::Vertex v1 = g.addVertex(1);
            G::Vertex v2 = g.addVertex(2);
            G::Vertex v3 = g.addVertex(3);
            G::Vertex v4 = g.addVertex(4);

            g.addEdge(v1, v2, 3);
            g.addEdge(v1, v3, 3);
            g.addEdge(v1, v4, 3);

            G::Edges edges = g.nextEdges(v2);

            EXPECT_THAT(g.nextEdges(v2), IsEmpty());
            EXPECT_THAT(g.nextEdges(v1), SizeIs(3));
        }
    }
}
