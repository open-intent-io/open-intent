#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>

#include "intent/OpenIntent.hpp"

#include "Domotic.hpp"

using namespace intent;

class StdOutputWriter : public OutputWriter
{
public:
    StdOutputWriter& operator<<(const std::string &output)
    {
        std::cout << output;
        return *this;
    }
};

class StdInputReader : public InputReader
{
public:
    StdInputReader& operator>>(std::string &input)
    {
        std::getline(std::cin, input);
        return *this;
    }
};

int main(int argc, char **argv)
{
    StdOutputWriter outputWriter;
    StdInputReader inputReader;


    Domotic::run(inputReader, outputWriter);
    return 0;
}

