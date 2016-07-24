#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>

#define GRAPH_DUMP_ENABLED

#include "intent/OpenIntent.hpp"

using namespace intent;



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

