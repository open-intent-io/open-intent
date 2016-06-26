#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>

#define GRAPH_DUMP_ENABLED

#include "intent/OpenIntent.hpp"

using namespace intent;


typedef boost::property_map<IntentStoryModel::StoryGraph::_Graph,
        std::string IntentStoryModel::VertexInfo::*>::type VertexActionIdPropertyMap;
typedef boost::property_map<IntentStoryModel::StoryGraph::_Graph,
        std::string IntentStoryModel::EdgeInfo::*>::type EdgeIntentIdPropertyMap;

struct VertexWriter
{
    VertexWriter(VertexActionIdPropertyMap actionId, IntentStoryModel &intentStoryModel)
            : m_actionId(actionId), m_intentStoryModel(intentStoryModel)
    {}

    template <class Vertex>
    void operator()(std::ostream &out, const Vertex& v) const {
        const std::string &actionId = m_actionId[v];

        out << "[label=\"" << actionId << "\"";

        if(m_intentStoryModel.isStateIdTerminal(actionId))
        {
            out << ", peripheries=2, color=\".7 .3 1.0\"";
        }

        out << "]";
    }

    VertexActionIdPropertyMap m_actionId;
    IntentStoryModel &m_intentStoryModel;
};

struct EdgeWriter
{
    EdgeWriter(EdgeIntentIdPropertyMap intentId, IntentStoryModel &intentStoryModel)
            : m_intentId(intentId), m_intentStoryModel(intentStoryModel)
    {}

    template <class Edge>
    void operator()(std::ostream &out, const Edge& e) const {
        const std::string &intentId = m_intentId[e];

        out << "[label=\"" + intentId + "\"]";
    }

    EdgeIntentIdPropertyMap m_intentId;
    IntentStoryModel &m_intentStoryModel;
};


struct WriterMaker
{
    WriterMaker(const WriterMaker &that) : m_intentStoryModel(that.m_intentStoryModel) {}
    WriterMaker(IntentStoryModel &intentStoryModel) : m_intentStoryModel(intentStoryModel) {}

    VertexWriter makeVertexWriter(IntentStoryModel::StoryGraph::_Graph &graph)
    {
        return VertexWriter(boost::get(&IntentStoryModel::VertexInfo::stateId, graph), m_intentStoryModel);
    }

    EdgeWriter makeEdgeWriter(IntentStoryModel::StoryGraph::_Graph &graph)
    {
        return EdgeWriter(boost::get(&IntentStoryModel::EdgeInfo::label, graph), m_intentStoryModel);
    }

    IntentStoryModel &m_intentStoryModel;
};


int main(int argc, char **argv)
{
    Deserializer deserializer;
    std::ifstream chatbot_json("chatbot.json");

    ChatbotModel chatbotModel = deserializer.deserialize<ChatbotModel>(chatbot_json);

    std::ofstream output_dot("output.dot");


    WriterMaker writerMaker(*chatbotModel.intentStoryServiceModel.intentStoryModel);
    chatbotModel.intentStoryServiceModel.intentStoryModel->graph.dump(output_dot, writerMaker);

    std::cout << "Dot file generated!";
    return 0;
}

