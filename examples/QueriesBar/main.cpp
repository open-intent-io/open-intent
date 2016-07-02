#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>

#include "intent/OpenIntent.hpp"

using namespace intent;

class Beverage;
typedef std::vector<Beverage> Menu;
typedef std::map<std::string, int> Cart;

static Menu menu;

class StdOutReply
{
public:
    void operator()(const std::string &message)
    {
        std::cout << message << std::endl;
    }
};

enum class DrinkCategory
{
    COCKTAIL,
    SOFT,
    BEER
};

enum class Quantifier {
    MINUS,
    PLUS,
    EQUAL
};

enum class DrinkAttributes {
    PRICE
};

enum class QuestionWord {
    WHAT,
    WHEN
};

struct Beverage {
    std::string name;
    float price;
    DrinkCategory category;

    std::ostringstream& operator<<(std::ostringstream& os)
    {
        os << this->name << " " << this->price << "€ \n";
        return os;
    }
    friend std::ostringstream& operator<<(std::ostringstream& os, const Beverage& beverage);
};

std::ostringstream& operator<<(std::ostringstream& os, const Beverage& beverage)
{
    os << beverage.name << " " << beverage.price << "€ \n";
    return os;
}

void extractBeverageQuery(const Chatbot::VariablesMap& intentVariables, Quantifier quantifier, DrinkAttributes& attribute, float& price)
{
    if (intentVariables.find("@quant0") != intentVariables.end())
        quantifier = Quantifier::MINUS;

    if (intentVariables.find("@attribute0") != intentVariables.end())
        attribute = DrinkAttributes::PRICE;

    Chatbot::VariablesMap::const_iterator it = intentVariables.find("@number0");
    if (it != intentVariables.end())
        price = std::stof(it->second);
}

void extractCategoryQuery(const Chatbot::VariablesMap& intentVariables, Quantifier& quantifier, DrinkCategory& category, float& price)
{
    if (intentVariables.find("@quant0") != intentVariables.end())
        quantifier = Quantifier::MINUS;

    Chatbot::VariablesMap::const_iterator it = intentVariables.find("@category0");
    if (it != intentVariables.end())
    {
        if (it->second == std::string("soft"))
            category = DrinkCategory::SOFT;
        if (it->second == std::string("biere"))
            category = DrinkCategory::BEER;
        if (it->second == std::string("cocktail"))
            category = DrinkCategory::COCKTAIL;
    }

    it = intentVariables.find("@number0");
    if (it != intentVariables.end())
        price = std::stof(it->second);

}

void extractPriceQuery(const Chatbot::VariablesMap& intentVariables, std::string& beverageName, int& number)
{
    Chatbot::VariablesMap::const_iterator it = intentVariables.find("@beverage0");
    if (it != intentVariables.end())
        beverageName = it->second;

    it = intentVariables.find("@number0");
    if (it != intentVariables.end())
        number = std::stoi(it->second);
}

std::string toString(std::vector<Beverage>& beverages)
{
    std::ostringstream oss;
    std::for_each(beverages.begin(), beverages.end(),
        [&oss](const Beverage& beverage){
            oss << beverage << std::endl;
        });
    return oss.str();
}

class ActionHandler : public intent::Chatbot::UserDefinedActionHandler
{
public:
    void operator()(const std::string &action, const Chatbot::VariablesMap &intentVariables,
                    Chatbot::VariablesMap &userDefinedVariables)
    {
        if(action == "#beveragequery")
        {
            Quantifier quant = Quantifier::PLUS;
            DrinkAttributes attribute = DrinkAttributes::PRICE;
            float price = 0;
            extractBeverageQuery(intentVariables, quant, attribute, price);
            std::vector<Beverage> beverages;
            if (quant == Quantifier::MINUS && attribute == DrinkAttributes::PRICE)
                std::copy_if(menu.begin(), menu.end(), std::back_inserter(beverages),
                    [price](const Beverage& beverage)
                    {
                        return beverage.price < price;
                    });
            if (!beverages.empty())
                userDefinedVariables["beverages"] = toString(beverages);
        }
        else if(action == "#pricequery")
        {
            float price = 0;
            bool found = false;
            std::string beverageName;
            int number = 0;
            extractPriceQuery(intentVariables, beverageName, number);
            std::for_each(menu.begin(), menu.end(), [&price,&found,&beverageName,&number](const Beverage& beverage) {
                if (beverage.name == beverageName)
                {
                    found = true;
                    price = beverage.price*number;
                }
            });

            std::ostringstream buff;
            if (found)
            {
                buff << price;
                userDefinedVariables["price"] = buff.str();
            }
        }
        else if(action == "#categoryquery")
        {
            DrinkCategory category;
            Quantifier quant = Quantifier::PLUS;
            float price = 0;
            extractCategoryQuery(intentVariables, quant, category, price);
            std::vector<Beverage> beverages;
            if (quant == Quantifier::MINUS)
                std::copy_if(menu.begin(), menu.end(), std::back_inserter(beverages),
                    [price, category](const Beverage& beverage)
                    {
                        return beverage.price < price && beverage.category == category;
                    });
            else
                std::copy_if(menu.begin(), menu.end(), std::back_inserter(beverages),
                    [price, category](const Beverage& beverage)
                    {
                        return beverage.price > price && beverage.category == category;
                    });
            if (!beverages.empty())
                userDefinedVariables["beverages"] = toString(beverages);
        }
    }
};


void fillMenu()
{
    menu.push_back({"Coca-Cola", 2.5f, DrinkCategory::SOFT});
    menu.push_back({"Heineken", 5.5f, DrinkCategory::BEER});
    menu.push_back({"Kronenbourg", 6.2f, DrinkCategory::BEER});
    menu.push_back({"Leffe", 5.8f, DrinkCategory::BEER});
    menu.push_back({"1664", 5.0f, DrinkCategory::BEER});
    menu.push_back({"Hoegarden", 5.0f, DrinkCategory::BEER});
    menu.push_back({"Mojito", 10.0f, DrinkCategory::COCKTAIL});
    menu.push_back({"Bloody-Mary", 10.0f, DrinkCategory::COCKTAIL});
}

int main(int argc, char **argv)
{
    fillMenu();

    std::cout << "##########################" << std::endl;
    std::cout << "#####   Query bar   #####" << std::endl;
    std::cout << "##########################" << std::endl;

    intent::Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler(new ActionHandler());

    std::ifstream modelFile("chatbot.json");
    intent::SingleSessionChatbot::SharedPtr chatbot =
            ChatbotFactory::createSingleSessionChatbotFromJsonModel(modelFile, userDefinedActionHandler);

    std::string inputLine;

    std::cout << "Quels [bieres | cocktails | soft] pour moins de 9" << std::endl;
    chatbot->treatMessage("Quels bieres pour moins de 9");
    chatbot->treatMessage("yes");

    std::cout << "Combien pour une Heineken ?"<< std::endl;
    chatbot->treatMessage("Combien pour une Heineken ?");
    chatbot->treatMessage("yes");

    std::cout << "plus de 3" << std::endl;
    chatbot->treatMessage("plus de 3");
    chatbot->treatMessage("yes");

    std::cout << "Tapez 'q' puis appuyez sur 'Entrer' pour quitter." << std::endl;
    while(inputLine != "q")
    {
        std::cout << "> ";
        std::getline(std::cin, inputLine);

        if(inputLine != "q")
        {
            chatbot->treatMessage(inputLine);
        }
    }
    std::cout << "Vous avez quitté la conversation.";
    return 0;
}

