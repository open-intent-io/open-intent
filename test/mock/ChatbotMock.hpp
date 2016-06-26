/*
|---------------------------------------------------------|
|    ___                   ___       _             _      |
|   / _ \ _ __   ___ _ __ |_ _|_ __ | |_ ___ _ __ | |_    |
|  | | | | '_ \ / _ \ '_ \ | || '_ \| __/ _ \ '_ \| __|   |
|  | |_| | |_) |  __/ | | || || | | | ||  __/ | | | |_    |
|   \___/| .__/ \___|_| |_|___|_| |_|\__\___|_| |_|\__|   |
|        |_|                                              |
|                                                         |
|     - The users first...                                |
|                                                         |
|     Authors:                                            |
|        - Clement Michaud                                |
|        - Sergei Kireev                                  |
|                                                         |
|     Version: 1.0.0                                      |
|                                                         |
|---------------------------------------------------------|

The MIT License (MIT)
Copyright (c) 2016 - Clement Michaud, Sergei Kireev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef INTENT_CHATBOTMOCK_HPP
#define INTENT_CHATBOTMOCK_HPP

#include "intent/chatbot/MultiSessionChatbot.hpp"

namespace intent
{
    namespace test
    {
        class ReplyHandlerMock : public Chatbot::ReplyActionHandler
        {
        public:
            MOCK_METHOD1(reply, void (const std::string &message));

            void operator()(const std::string &message)
            {
                reply(message);
            }
        };

        class UserDefinedCommandMock : public Chatbot::UserDefinedActionHandler
        {
        public:
            MOCK_METHOD3(execute, void (const std::string &action, const Chatbot::VariablesMap &immutableVariables,
                    Chatbot::VariablesMap &mutableVariables));

            void operator()(const std::string &action,
                            const Chatbot::VariablesMap &immutableVariables,
                            Chatbot::VariablesMap &mutableVariables)
            {
                execute(action, immutableVariables, mutableVariables);
            }
        };




        class MultiSessionReplyHandlerMock :
                public MultiSessionChatbot<std::string>::ReplyActionHandler
        {
        public:
            MOCK_METHOD2(reply, void (const std::string &sessionId, const std::string &message));

            void operator()(const std::string &sessionId, const std::string &message)
            {
                reply(sessionId, message);
            }
        };

        class MultiSessionUserDefinedCommandMock :
                public MultiSessionChatbot<std::string>::UserDefinedActionHandler
        {
        public:
            MOCK_METHOD4(execute, void (const std::string &sessionId, const std::string &action,
                    const Chatbot::VariablesMap &immutableVariables,
                    Chatbot::VariablesMap &mutableVariables));

            void operator()(const std::string &sessionId,
                            const std::string &action,
                            const Chatbot::VariablesMap &immutableVariables,
                            Chatbot::VariablesMap &mutableVariables)
            {
                execute(sessionId, action, immutableVariables, mutableVariables);
            }
        };
    }
}

#endif //INTENT_CHATBOTMOCK_HPP
