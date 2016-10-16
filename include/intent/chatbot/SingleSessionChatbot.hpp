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
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef INTENT_SINGLESESSIONCHATBOT_HPP
#define INTENT_SINGLESESSIONCHATBOT_HPP

#include "intent/chatbot/Chatbot.hpp"

#include <string>

namespace intent {
/**
 * \brief An implementation of Chatbot that uses a single session.
 */
class SingleSessionChatbot : protected Chatbot {
 public:
  typedef std::shared_ptr<SingleSessionChatbot> SharedPtr;

  /**
   * \param chatbotModel              The model to be used.
   * \param replyActionHandler        The user implementation of the
   * ReplyActionHandler
   * \param userDefinedActionHandler  The user implementation of the
   * UserDefinedActionHandler
   */
  SingleSessionChatbot(
      const ChatbotModel& chatbotModel,
      Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler);

  /**
   * \brief Handles a user intent and reply or perform actions
   * \param message   The user message.
   * \return The replies built by the chatbot
   */
  std::vector<std::string> treatMessage(const std::string& message);

  /**
   * \brief   Returns the single session context.
   * \return  The session context, i.e., the state ID and the user defined
   * variables.
   */
  Context getContext() const { return m_context; }

 private:
  /**
   * \brief The session context, i.e., the state ID and the user defined
   * variables.
   */
  Chatbot::Context m_context;

  /**
   * The UserDefinedActionHandler provided by the user that will be called when
   * a user defined action
   * must be executed
   */
  UserDefinedActionHandler::SharedPtr m_userDefinedActionHandler;

 private:
  class UserDefinedActionHandlerAdapter
      : public Chatbot::UserDefinedActionHandler {
   public:
    UserDefinedActionHandlerAdapter(const std::string& state,
        SingleSessionChatbot& chatbot, std::vector<std::string>& replies,
        typename Chatbot::UserDefinedActionHandler::SharedPtr
            userDefinedActionHandler)
        : m_state(state),
          m_userDefinedActionHandler(userDefinedActionHandler),
          m_chatbot(chatbot),
          m_replies(replies) {}

    void operator()(const std::string& action,
                    const Chatbot::VariablesMap& intentVariables,
                    Chatbot::VariablesMap& userDefinedVariables) {
      (*m_userDefinedActionHandler)(action, intentVariables,
                                    userDefinedVariables);
      m_replies = m_chatbot.prepareReplies(m_state, action, intentVariables,
                                           userDefinedVariables);
    }

   private:
      const std::string m_state;
    typename Chatbot::UserDefinedActionHandler::SharedPtr
        m_userDefinedActionHandler;
    SingleSessionChatbot& m_chatbot;
    std::vector<std::string>& m_replies;
  };
};
}

#endif  // INTENT_SINGLESESSIONCHATBOT_HPP
