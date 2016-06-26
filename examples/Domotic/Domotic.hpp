//
// Created by clement on 29/05/16.
//

#ifndef INTENT_DOMOTIC_HPP
#define INTENT_DOMOTIC_HPP

#include <istream>
#include <ostream>
#include <string>

class InputReader
{
public:
    virtual InputReader& operator>>(std::string &input) = 0;
};

class OutputWriter
{
public:
    virtual OutputWriter& operator<<(const std::string &output) = 0;
};

class Domotic
{
public:
    static void run(InputReader &inputReader, OutputWriter &outputWriter);
};


#endif //INTENT_DOMOTIC_HPP
