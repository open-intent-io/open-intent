#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <queue>

#include "Domotic.hpp"

using namespace ::testing;

class InputReaderMock : public InputReader
{
public:
    InputReaderMock(const std::queue<std::string> &scenario)
    : m_scenarii(scenario)
    {
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

class DomoticTest : public ::testing::Test
{
public:

    void expectStartOutput(OutputWriterMock &outputWriterMock)
    {

        EXPECT_CALL(outputWriterMock, write("###########################\n"));
        EXPECT_CALL(outputWriterMock, write("#######   Domotic   #######\n"));
        EXPECT_CALL(outputWriterMock, write("###########################\n"));

        EXPECT_CALL(outputWriterMock, write("Hello I'm Bob\n"
                                                    "I'm your virtual assistant and I will help you "
                                                    "open or close the window, turn on and off the light "
                                                    "or still lower or higher the heating system"));

        EXPECT_CALL(outputWriterMock, write("Type 'q' and 'Enter' to quit.\n\n"));
    }
    void expectEndOutput(OutputWriterMock &outputWriterMock)
    {
        EXPECT_CALL(outputWriterMock, write("What do you want me to do?\n"));
        EXPECT_CALL(outputWriterMock, write("See ya!"));
    }
};

TEST_F(DomoticTest, turn_the_light_on)
{
    std::queue<std::string> scenario;
    scenario.push("Turn the light on, please!");
    scenario.push("q");

    InputReaderMock inputReaderMock(scenario);
    OutputWriterMock outputWriterMock;

    {
        InSequence s;

        expectStartOutput(outputWriterMock);
        EXPECT_CALL(outputWriterMock, write("What do you want me to do?\n"));
        EXPECT_CALL(outputWriterMock, write("Light is on."));
        EXPECT_CALL(outputWriterMock, write("\n"));
        expectEndOutput(outputWriterMock);
    }

    Domotic::run(inputReaderMock, outputWriterMock);
}

TEST_F(DomoticTest, turn_the_light_off)
{
    std::queue<std::string> scenario;
    scenario.push("Turn off the light, please!");
    scenario.push("q");

    InputReaderMock inputReaderMock(scenario);
    OutputWriterMock outputWriterMock;

    {
        InSequence s;
        expectStartOutput(outputWriterMock);
        EXPECT_CALL(outputWriterMock, write("What do you want me to do?\n"));
        EXPECT_CALL(outputWriterMock, write("Light is off."));
        EXPECT_CALL(outputWriterMock, write("\n"));
        expectEndOutput(outputWriterMock);
    }

    Domotic::run(inputReaderMock, outputWriterMock);
}

TEST_F(DomoticTest, open_the_window)
{
    std::queue<std::string> scenario;
    scenario.push("Open the window, please!");
    scenario.push("q");

    InputReaderMock inputReaderMock(scenario);
    OutputWriterMock outputWriterMock;

    {
        InSequence s;
        expectStartOutput(outputWriterMock);
        EXPECT_CALL(outputWriterMock, write("What do you want me to do?\n"));
        EXPECT_CALL(outputWriterMock, write("Window is open."));
        EXPECT_CALL(outputWriterMock, write("\n"));
        expectEndOutput(outputWriterMock);
    }

    Domotic::run(inputReaderMock, outputWriterMock);
}

TEST_F(DomoticTest, close_the_window)
{
    std::queue<std::string> scenario;
    scenario.push("Close the window, please!");
    scenario.push("q");

    InputReaderMock inputReaderMock(scenario);
    OutputWriterMock outputWriterMock;

    {
        InSequence s;
        expectStartOutput(outputWriterMock);
        EXPECT_CALL(outputWriterMock, write("What do you want me to do?\n"));
        EXPECT_CALL(outputWriterMock, write("Window is closed."));
        EXPECT_CALL(outputWriterMock, write("\n"));
        expectEndOutput(outputWriterMock);
    }

    Domotic::run(inputReaderMock, outputWriterMock);
}

TEST_F(DomoticTest, lower_the_heating_system)
{
    std::queue<std::string> scenario;
    scenario.push("Lower the heating system");
    scenario.push("q");

    InputReaderMock inputReaderMock(scenario);
    OutputWriterMock outputWriterMock;

    {
        InSequence s;
        expectStartOutput(outputWriterMock);
        EXPECT_CALL(outputWriterMock, write("What do you want me to do?\n"));
        EXPECT_CALL(outputWriterMock, write("Heating system has been lowered."));
        EXPECT_CALL(outputWriterMock, write("\n"));
        expectEndOutput(outputWriterMock);
    }

    Domotic::run(inputReaderMock, outputWriterMock);
}

TEST_F(DomoticTest, higher_the_heating_system)
{
    std::queue<std::string> scenario;
    scenario.push("Higher the heating system");
    scenario.push("q");

    InputReaderMock inputReaderMock(scenario);
    OutputWriterMock outputWriterMock;

    {
        InSequence s;
        expectStartOutput(outputWriterMock);
        EXPECT_CALL(outputWriterMock, write("What do you want me to do?\n"));
        EXPECT_CALL(outputWriterMock, write("Heating system has been highered."));
        EXPECT_CALL(outputWriterMock, write("\n"));
        expectEndOutput(outputWriterMock);
    }

    Domotic::run(inputReaderMock, outputWriterMock);
}

