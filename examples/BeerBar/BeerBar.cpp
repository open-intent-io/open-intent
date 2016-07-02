//
// Created by clement on 29/05/16.
//

#include "BeerBar.hpp"

#include "intent/OpenIntent.hpp"

using namespace intent;

typedef std::map<std::string, float> Menu;
typedef std::map<std::string, int> Cart;

static Menu menu;

class ActionHandler : public intent::Chatbot::UserDefinedActionHandler
{
public:
    void insertItem(const Chatbot::VariablesMap &intentVariables, const std::string &number, const std::string &beverage)
    {
        Chatbot::VariablesMap::const_iterator number0It = intentVariables.find(number);
        Chatbot::VariablesMap::const_iterator beverage0It = intentVariables.find(beverage);

        if(number0It != intentVariables.end() &&
           beverage0It != intentVariables.end())
        {
            std::string number0 = number0It->second;
            std::string beverage0 = beverage0It->second;

            std::map<std::string, int>::iterator itemIt = m_cart.find(beverage0);
            if(itemIt != m_cart.end())
            {
                itemIt->second += std::stoi(number0);
            }
            else
            {
                m_cart.insert(std::make_pair(beverage0, std::stoi(number0)));
            }
        }
    }

    void operator()(const std::string &action, const Chatbot::VariablesMap &intentVariables,
                    Chatbot::VariablesMap &userDefinedVariables)
    {
        if(action == "append_order1" || action == "append_order2")
        {
            insertItem(intentVariables, "@number0", "@beverage0");
            insertItem(intentVariables, "@number1", "@beverage1");
        }
        else if(action == "order_and_grab_it")
        {
            std::string orderDescription;
            double price = 0.0;
            for(const std::pair<std::string, int> &item: m_cart)
            {
                Menu::const_iterator itemPriceIt = menu.find(item.first);
                if(itemPriceIt != menu.end())
                {
                    orderDescription += std::to_string(item.second) + "x " + item.first + ", ";
                    price += itemPriceIt->second * item.second;
                }
            }
            orderDescription = orderDescription.substr(0, orderDescription.length()-2);

            std::stringstream ss;
            ss.precision(2);
            ss << std::fixed << price;

            userDefinedVariables.insert(std::make_pair("price", ss.str()));
            userDefinedVariables.insert(std::make_pair("order_description", orderDescription));
        }
        else if(action == "bye")
        {
            m_cart.clear();
        }
    }

private:
    Cart m_cart;
};


void fillMenu()
{
    menu.insert(std::make_pair("Coca-Cola", 2.5f));
    menu.insert(std::make_pair("Kronenbourg", 6.2f));
    menu.insert(std::make_pair("Heineken", 5.5f));
    menu.insert(std::make_pair("Leffe", 5.8f));
    menu.insert(std::make_pair("Hoegarden", 5.0f));
    menu.insert(std::make_pair("1664", 4.0f));
    menu.insert(std::make_pair("Pale ale", 5.7f));
    menu.insert(std::make_pair("Duff", 4.8f));
    menu.insert(std::make_pair("Amstel", 5.1f));
    menu.insert(std::make_pair("Pietra", 7.0f));
    menu.insert(std::make_pair("Pelforth", 5.4f));
}

void BeerBar::run(InputReader &istream, OutputWriter &ostream)
{
    fillMenu();

    ostream << "##########################\n";
    ostream << "#######  Beer Bar  #######\n";
    ostream << "##########################\n";

    intent::Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(new ActionHandler());

    std::ifstream modelFile("chatbot.json");
    intent::SingleSessionChatbot::SharedPtr chatbot =
            ChatbotFactory::createSingleSessionChatbotFromJsonModel(modelFile, userDefinedActionHandler);
    std::string inputLine;
    ostream << "Say \"Hello Bob!\"\n"
                "Type 'q' then  'Enter' to quit.\n";
    while(inputLine != "q")
    {
        ostream << "> ";
        istream >> inputLine;

        if(inputLine != "q")
        {
            ostream << chatbot->treatMessage(inputLine);
        }
    }
    ostream << "You left the conversation.\n";
}