#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>

#include "intent/OpenIntent.hpp"

using namespace intent;



int main(int argc, char **argv)
{
    Deserializer deserializer;
    std::ifstream chatbot_json("chatbot.json");

    ChatbotModel chatbotModel = deserializer.deserialize<ChatbotModel>(chatbot_json);

    std::ofstream output_dot("output.dot");

    intent::IntentStoryModelSerializer serializer;
    serializer.serialize(output_dot, chatbotModel.intentStoryServiceModel);

    std::cout << "Dot file generated!";
    return 0;
}

