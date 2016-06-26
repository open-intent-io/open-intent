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
        write(output);
        return *this;
    }

    MOCK_METHOD1(write, void (const std::string &input));
};

class BeerBarTest : public ::testing::Test
{
public:

    void expectStartOutput(OutputWriterMock &outputWriterMock)
    {
        EXPECT_CALL(outputWriterMock, write("##########################\n"));
        EXPECT_CALL(outputWriterMock, write("#######  Beer Bar  #######\n"));
        EXPECT_CALL(outputWriterMock, write("##########################\n"));

        EXPECT_CALL(outputWriterMock, write("Say \"Hello Bob!\"\n"
                                                    "Type 'q' then  'Enter' to quit.\n"));
    }
    void expectEndOutput(OutputWriterMock &outputWriterMock)
    {
        EXPECT_CALL(outputWriterMock, write("You left the conversation.\n"));
    }
};

TEST_F(BeerBarTest, order_one_coca_cola)
{
    InputReaderMock inputReaderMock;
    OutputWriterMock outputWriterMock;

    {
        InSequence s;
        expectStartOutput(outputWriterMock);

        EXPECT_CALL(outputWriterMock, write("> "));
        inputReaderMock.reply("Bob");

        EXPECT_CALL(outputWriterMock, write("What can I get you?\n"));
        EXPECT_CALL(outputWriterMock, write("> "));
        inputReaderMock.reply("One Coca-Cola");

        EXPECT_CALL(outputWriterMock, write("I'm adding 1 Coca-Cola. Would you like something else?\n"));
        EXPECT_CALL(outputWriterMock, write("> "));
        inputReaderMock.reply("No");

        EXPECT_CALL(outputWriterMock, write("Here is what you ordered: 1x Coca-Cola.\n"));
        EXPECT_CALL(outputWriterMock, write("Please come grab your order. It's gonna be 2.50$.\n"));
        EXPECT_CALL(outputWriterMock, write("Goodbye.\n"));
        EXPECT_CALL(outputWriterMock, write("> "));
        inputReaderMock.reply("q");

        expectEndOutput(outputWriterMock);
    }

    BeerBar::run(inputReaderMock, outputWriterMock);
}

TEST_F(BeerBarTest, error_while_ordering)
{
    InputReaderMock inputReaderMock;
    OutputWriterMock outputWriterMock;

    {
        InSequence s;
        expectStartOutput(outputWriterMock);

        EXPECT_CALL(outputWriterMock, write("> "));
        inputReaderMock.reply("Bob");

        EXPECT_CALL(outputWriterMock, write("What can I get you?\n"));
        EXPECT_CALL(outputWriterMock, write("> "));
        inputReaderMock.reply("One Cora");

        EXPECT_CALL(outputWriterMock, write("I did not get it, what do you want to order?\n"));
        EXPECT_CALL(outputWriterMock, write("> "));
        inputReaderMock.reply("Two Coca");

        EXPECT_CALL(outputWriterMock, write("I'm adding 2 Coca-Cola. Would you like something else?\n"));
        EXPECT_CALL(outputWriterMock, write("> "));
        inputReaderMock.reply("No");

        EXPECT_CALL(outputWriterMock, write("Here is what you ordered: 2x Coca-Cola.\n"));
        EXPECT_CALL(outputWriterMock, write("Please come grab your order. It's gonna be 5.00$.\n"));
        EXPECT_CALL(outputWriterMock, write("Goodbye.\n"));
        EXPECT_CALL(outputWriterMock, write("> "));
        inputReaderMock.reply("q");

        expectEndOutput(outputWriterMock);
    }

    BeerBar::run(inputReaderMock, outputWriterMock);
}

TEST_F(BeerBarTest, do_not_order_anything)
{
    InputReaderMock inputReaderMock;
    OutputWriterMock outputWriterMock;

    {
        InSequence s;
        expectStartOutput(outputWriterMock);

        EXPECT_CALL(outputWriterMock, write("> "));
        inputReaderMock.reply("Bob");

        EXPECT_CALL(outputWriterMock, write("What can I get you?\n"));
        EXPECT_CALL(outputWriterMock, write("> "));
        inputReaderMock.reply("nothing");

        EXPECT_CALL(outputWriterMock, write("Goodbye.\n"));
        EXPECT_CALL(outputWriterMock, write("> "));
        inputReaderMock.reply("q");

        expectEndOutput(outputWriterMock);
    }

    BeerBar::run(inputReaderMock, outputWriterMock);
}