#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <queue>

#include "BeerBar.hpp"

using namespace ::testing;

class InputReaderMock : public InputReader
{
public:
    InputReaderMock()
    {
    }

    void reply(const std::string &reply)
    {
        m_scenarii.push(reply);
    }

    InputReader& operator>>(std::string &input)
    {
        input = std::string(m_scenarii.front());
        m_scenarii.pop();
        return *this;
    }

private:
    std::queue<std::string> m_scenarii;
};

class OutputWriterMock : public OutputWriter
{
public:
    OutputWriter& operator<<(const std::string &output)
    {
        m_replies.push_back(output);
        return *this;
    }

    OutputWriter& operator<<(const std::vector<std::string>& replies)
    {
        for(const std::string &reply: replies) {
            m_replies.push_back(reply + "\n");
        }
        return *this;
    }

    std::vector<std::string> m_replies;
};

class BeerBarTest : public ::testing::Test
{
public:

    void expectStartOutput(OutputWriterMock &outputWriterMock, std::vector<std::string> &expectedOutput)
    {
        expectedOutput.push_back("##########################\n");
        expectedOutput.push_back("#######  Beer Bar  #######\n");
        expectedOutput.push_back("##########################\n");

        expectedOutput.push_back("Say \"Hello Bob!\"\n"
                                         "Type 'q' then  'Enter' to quit.\n");
    }

    void expectEndOutput(OutputWriterMock &outputWriterMock, std::vector<std::string> &expectedOutput)
    {
        expectedOutput.push_back("You left the conversation.\n");
    }
};

TEST_F(BeerBarTest, order_one_coca_cola)
{
    std::vector<std::string> expectedOutput;
    InputReaderMock inputReaderMock;
    OutputWriterMock outputWriterMock;

    {
        InSequence s;
        expectStartOutput(outputWriterMock, expectedOutput);

        expectedOutput.push_back("> ");
        inputReaderMock.reply("Bob");

        expectedOutput.push_back("What can I get you?\n");
        expectedOutput.push_back("> ");
        inputReaderMock.reply("One Coca-Cola");

        expectedOutput.push_back("I'm adding 1 Coca-Cola. Would you like something else?\n");
        expectedOutput.push_back("> ");
        inputReaderMock.reply("No");

        expectedOutput.push_back("Here is what you ordered: 1x Coca-Cola.\n");
        expectedOutput.push_back("Please come grab your order. It's gonna be 2.50$.\n");
        expectedOutput.push_back("Goodbye.\n");
        expectedOutput.push_back("> ");
        inputReaderMock.reply("q");

        expectEndOutput(outputWriterMock, expectedOutput);
    }

    BeerBar::run(inputReaderMock, outputWriterMock);

    EXPECT_EQ(expectedOutput, outputWriterMock.m_replies);
}

TEST_F(BeerBarTest, error_while_ordering)
{
    std::vector<std::string> expectedOutput;
    InputReaderMock inputReaderMock;
    OutputWriterMock outputWriterMock;

    {
        InSequence s;
        expectStartOutput(outputWriterMock, expectedOutput);

        expectedOutput.push_back("> ");
        inputReaderMock.reply("Bob");

        expectedOutput.push_back("What can I get you?\n");
        expectedOutput.push_back("> ");
        inputReaderMock.reply("One Cora");

        expectedOutput.push_back("I did not get it, what do you want to order?\n");
        expectedOutput.push_back("> ");
        inputReaderMock.reply("Two Coca");

        expectedOutput.push_back("I'm adding 2 Coca-Cola. Would you like something else?\n");
        expectedOutput.push_back("> ");
        inputReaderMock.reply("No");

        expectedOutput.push_back("Here is what you ordered: 2x Coca-Cola.\n");
        expectedOutput.push_back("Please come grab your order. It's gonna be 5.00$.\n");
        expectedOutput.push_back("Goodbye.\n");
        expectedOutput.push_back("> ");
        inputReaderMock.reply("q");

        expectEndOutput(outputWriterMock, expectedOutput);
    }

    BeerBar::run(inputReaderMock, outputWriterMock);

    EXPECT_EQ(expectedOutput, outputWriterMock.m_replies);
}

TEST_F(BeerBarTest, do_not_order_anything)
{
    std::vector<std::string> expectedOutput;
    InputReaderMock inputReaderMock;
    OutputWriterMock outputWriterMock;

    {
        InSequence s;
        expectStartOutput(outputWriterMock, expectedOutput);

        expectedOutput.push_back("> ");
        inputReaderMock.reply("Bob");

        expectedOutput.push_back("What can I get you?\n");
        expectedOutput.push_back("> ");
        inputReaderMock.reply("nothing");

        expectedOutput.push_back("Goodbye.\n");
        expectedOutput.push_back("> ");
        inputReaderMock.reply("q");

        expectEndOutput(outputWriterMock, expectedOutput);
    }

    BeerBar::run(inputReaderMock, outputWriterMock);

    EXPECT_EQ(expectedOutput, outputWriterMock.m_replies);
}