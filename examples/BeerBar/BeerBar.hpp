//
// Created by clement on 29/05/16.
//

#ifndef INTENT_BEERBAR_HPP
#define INTENT_BEERBAR_HPP

#include <istream>
#include <ostream>
#include <string>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>


class InputReader
{
public:
    virtual InputReader& operator>>(std::string &input) = 0;
};

class OutputWriter
{
public:
    virtual OutputWriter& operator<<(const std::string &output) = 0;
    virtual OutputWriter& operator<<(const std::vector<std::string> &replies) = 0;
};

class BeerBar
{
public:
    static void run(InputReader &inputReader, OutputWriter &outputWriter);
};


#endif //INTENT_BEERBAR_HPP
