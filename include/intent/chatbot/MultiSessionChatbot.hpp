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
#ifndef INTENT_MULTISESSIONCHATBOT_HPP
#define INTENT_MULTISESSIONCHATBOT_HPP

#include "intent/chatbot/Chatbot.hpp"
#include "intent/chatbot/ChatbotModel.hpp"

#include <map>
#include <memory>

namespace intent {
/**
 * \brief An implementation of Chatbot that uses a multiple sessions registered
 * by the user.
 */
template <typename SessionIdType>
class MultiSessionChatbot : protected Chatbot {
 public:
  typedef std::shared_ptr<MultiSessionChatbot> SharedPtr;

  /**
   * \brief The specific UserDefinedActionHandler for multi session chatbots
   */
  class UserDefinedActionHandler {
   public:
    virtual void operator()(
        const SessionIdType& sessionId, const std::string& action,
        const Chatbot::VariablesMap& intentVariables,
        Chatbot::VariablesMap& templateRepliesVariables) = 0;

    typedef std::shared_ptr<UserDefinedActionHandler> SharedPtr;
  };

  /**
   * \param chatbotModel              The data model describing the dictionary,
   * the intent model and the actions.
   * \param userDefinedActionHandler  The user implemenation of a
   * UserDefinedActionHandler.
   */
  MultiSessionChatbot(
      const ChatbotModel& chatbotModel,
      typename UserDefinedActionHandler::SharedPtr userDefinedActionHandler);

  /**
   * \brief Handles a user intent for a particular session and reply or perform
   * actions
   * \param sessionId The session ID of the session
   * \param message   The user message.
   * \return The replies built by the chatbot
   */
  std::vector<std::string> treatMessage(const SessionIdType& sessionId,
                                        const std::string& message);

  /**
   * \brief Register a session in the chatbot.
   * \param sessionId     The session ID.
   */
  void addSession(const SessionIdType& sessionId);

  /**
   * \brief Unregister a session from the chatbot.
   * \param sessionId     The session ID.
   */
  void removeSession(const SessionIdType& sessionId);

  /**
   * \brief Return the number of registered sessions.
   * \return The number of registered sessions.
   */
  inline size_t sessionCount() const { return m_sessionIndex.size(); }

 private:
  using SessionIndex = std::map<SessionIdType, Chatbot::Context>;

  /**
   * \brief The data model used by the Chatbot.
   */
  const ChatbotModel m_chatbotModel;

  /**
   * \brief The index of contexts by session ID.
   */
  SessionIndex m_sessionIndex;

  /**
   * The UserDefinedActionHandler provided by the user that will be called when
   * a user defined action
   * must be executed
   */
  typename UserDefinedActionHandler::SharedPtr m_userDefinedActionHandler;

 private:
  class UserDefinedActionHandlerAdapter
      : public Chatbot::UserDefinedActionHandler {
   public:
    UserDefinedActionHandlerAdapter(
        MultiSessionChatbot& chatbot, std::vector<std::string>& replies,
        const SessionIdType& sessionId,
        typename MultiSessionChatbot::UserDefinedActionHandler::SharedPtr
            userDefinedActionHandler)
        : m_userDefinedActionHandler(userDefinedActionHandler),
          m_sessionId(sessionId),
          m_chatbot(chatbot),
          m_replies(replies) {}

    void operator()(const std::string& action,
                    const Chatbot::VariablesMap& intentVariables,
                    Chatbot::VariablesMap& userDefinedVariables) {
      (*m_userDefinedActionHandler)(m_sessionId, action, intentVariables,
                                    userDefinedVariables);
      m_replies = m_chatbot.prepareReplies(action, intentVariables,
                                           userDefinedVariables);
    }

   private:
    typename MultiSessionChatbot::UserDefinedActionHandler::SharedPtr
        m_userDefinedActionHandler;
    SessionIdType m_sessionId;
    std::vector<std::string>& m_replies;
    MultiSessionChatbot& m_chatbot;
  };
};
}

#include "MultiSessionChatbot.inl.hpp"

#endif  // INTENT_MULTISESSIONCHATBOT_HPP
