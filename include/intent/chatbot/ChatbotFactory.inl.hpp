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
#ifndef INTENT_CHATBOTFACTORY_INL_HPP
#define INTENT_CHATBOTFACTORY_INL_HPP

#include "ChatbotModel.hpp"
#include "MultiSessionChatbot.hpp"
#include "intent/interpreter/Interpreter.hpp"

namespace intent {

template <typename SessionIdType>
typename MultiSessionChatbot<SessionIdType>::SharedPtr
ChatbotFactory::createMultiSessionChatbotFromJsonModel(
    std::istream& model,
    typename MultiSessionChatbot<SessionIdType>::ReplyActionHandler::SharedPtr
        replyActionHandler,
    typename MultiSessionChatbot<SessionIdType>::UserDefinedActionHandler::
        SharedPtr userDefinedActionHandler) {
  typename intent::MultiSessionChatbot<SessionIdType>::SharedPtr chatbot;

  ChatbotModel chatbotModel;
  if (ChatbotFactory::loadFromJsonModel(model, chatbotModel)) {
    chatbot.reset(new intent::MultiSessionChatbot<SessionIdType>(
        chatbotModel, replyActionHandler, userDefinedActionHandler));
  }
  return chatbot;
}

template <typename SessionIdType>
typename MultiSessionChatbot<SessionIdType>::SharedPtr
ChatbotFactory::createMultiSessionChatbotFromOIML(
    std::istream& dictionaryModel, std::istream& interpreterModel,
    typename MultiSessionChatbot<SessionIdType>::ReplyActionHandler::SharedPtr
        replyActionHandler,
    typename MultiSessionChatbot<SessionIdType>::UserDefinedActionHandler::
        SharedPtr userDefinedActionHandler) {
  typename intent::MultiSessionChatbot<SessionIdType>::SharedPtr chatbot;

  ChatbotModel chatbotModel;
  InterpreterFeedback feedback;
  if (ChatbotFactory::loadFromOIML(dictionaryModel, interpreterModel,
                                   chatbotModel, feedback)) {
    chatbot.reset(new intent::MultiSessionChatbot<SessionIdType>(
        chatbotModel, replyActionHandler, userDefinedActionHandler));
  }
  return chatbot;
}
}

#endif  // INTENT_CHATBOTFACTORY_INL_HPP_HPP
