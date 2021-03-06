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
#ifndef INTENT_MULTISESSIONCHATBOT_INL_HPP
#define INTENT_MULTISESSIONCHATBOT_INL_HPP

namespace intent {
template <typename SessionIdType>
MultiSessionChatbot<SessionIdType>::MultiSessionChatbot(
    const ChatbotModel& chatbotModel,
    typename UserDefinedActionHandler::SharedPtr userDefinedActionHandler)
    : Chatbot(chatbotModel),
      m_userDefinedActionHandler(userDefinedActionHandler) {}

template <typename SessionIdType>
std::vector<std::string> MultiSessionChatbot<SessionIdType>::treatMessage(
    const SessionIdType& sessionId, const std::string& message) {
  typename SessionIndex::iterator it = m_sessionIndex.find(sessionId);

  std::vector<std::string> replies;

  if (it != m_sessionIndex.end()) {
    UserDefinedActionHandlerAdapter actionHandler(it->second.currentStateId,
                                                  *this, replies, it->first,
                                                  m_userDefinedActionHandler);

    Chatbot::VariablesMap userDefinedVariables;
    Chatbot::VariablesMap intentVariables;
    std::string currentStateId = it->second.currentStateId;
    Chatbot::treatMessage(message, it->second, actionHandler, intentVariables,
                          userDefinedVariables);
  }
  return replies;
}

template <typename SessionIdType>
void MultiSessionChatbot<SessionIdType>::addSession(
    const SessionIdType& sessionId) {
  Chatbot::Context sessionContext;
  sessionContext.currentStateId =
      m_intentStoryService.getIntentStoryServiceModel()
          .intentStoryModel->rootStateId;
  m_sessionIndex.insert(std::make_pair(sessionId, sessionContext));
}

template <typename SessionIdType>
void MultiSessionChatbot<SessionIdType>::removeSession(
    const SessionIdType& sessionId) {
  m_sessionIndex.erase(sessionId);
}
}

#endif  // INTENT_MULTISESSIONCHATBOT_INL_HPP
