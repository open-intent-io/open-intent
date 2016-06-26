//
// Created by clement on 29/05/16.
//

#include "Domotic.hpp"

#include "intent/OpenIntent.hpp"

#include <fstream>

using namespace intent;

void treatIntent(const IntentService::Result &intentResult, OutputWriter &ostream)
{
    if(intentResult.intent.intentId == "turn_the_light_X")
    {
        if(intentResult.intent.entityMatches[1].term == "on")
        {
            ostream << "Light is on.";
        }
        else
        {
            ostream << "Light is off.";
        }
    }
    else if(intentResult.intent.intentId == "turn_X_the_light")
    {
        if(intentResult.intent.entityMatches[0].term == "on")
        {
            ostream << "Light is on.";
        }
        else
        {
            ostream << "Light is off.";
        }
    }
    else if(intentResult.intent.intentId == "open_close_window")
    {
        if(intentResult.intent.entityMatches[0].term == "open")
        {
            ostream << "Window is open.";
        }
        else
        {
            ostream << "Window is closed.";
        }
    }
    else if(intentResult.intent.intentId == "lower_higher_heating")
    {
        if(intentResult.intent.entityMatches[0].term == "lower")
        {
            ostream << "Heating system has been lowered.";
        }
        else
        {
            ostream << "Heating system has been highered.";
        }
    }
}

void Domotic::run(InputReader &istream, OutputWriter &ostream)
{
    ostream << "###########################\n";
    ostream << "#######   Domotic   #######\n";
    ostream << "###########################\n";

    std::string dictionaryFilename = "intent.json";
    std::ifstream f(dictionaryFilename);
    intent::Deserializer deserializer;

    IntentServiceModel intentServiceModel = deserializer.deserialize<IntentServiceModel>(f);
    intent::IntentService intentService(intentServiceModel);

    std::string inputLine;

    ostream << "Hello I'm Bob\n"
                       "I'm your virtual assistant and I will help you "
                       "open or close the window, turn on and off the light "
                       "or still lower or higher the heating system";
    ostream << "Type 'q' and 'Enter' to quit.\n\n";
    while(inputLine != "q")
    {
        ostream << "What do you want me to do?\n";
        istream >> inputLine;

        if(inputLine != "q")
        {
            IntentService::Result result = intentService.evaluate(inputLine);
            if(result.found)
            {
                treatIntent(result, ostream);
                ostream << "\n";
            }
            else
            {
                ostream << "I did not understand, please make it understandable to me :)\n";
            }
        }
    }
    ostream << "See ya!";
}