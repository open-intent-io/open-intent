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
#ifndef INTENT_CHATBOTFACTORY_HPP
#define INTENT_CHATBOTFACTORY_HPP

#include "MultiSessionChatbot.hpp"
#include "SingleSessionChatbot.hpp"
#include "intent/interpreter/Interpreter.hpp"

namespace intent {
/**
 * \brief An helper factory that creates a chatbot easily.
 */
class ChatbotFactory {
 public:
  /**
   * \param model             The data model to be loaded in the chatbot.
   * \return A Chatbot.
   *
   * \brief Create a chatbot from a dictionary model
   */
  static Chatbot::SharedPtr createChatbotFromJsonModel(std::istream& model);

  /**
   * \param dictionaryModel             The data model to be loaded in the
   * chatbot.
   * \param interpreterModel            The data model describing the dialogs
   * enabled by the chatbot.
   * \param feedback                    The feedback of the interpreter
   * \return A Chatbot.
   *
   * \brief Create a chatbot from an OpenIntent markup language, a dictionary
   * model
   */
  static Chatbot::SharedPtr createChatbotFromOIML(
      std::istream& dictionaryModel, std::istream& interpreterModel,
      InterpreterFeedback& feedback);

  /**
   * \param model             The data model to be loaded in the chatbot.
   * \param userDefinedActionHandler  The user implementation of the
   * userDefinedActionHandler that will be used
   * by the chatbot.
   * \return A MultiSessionChatbot.
   *
   * \brief Create a single session chatbot with the datamodel contained in
   * modelFilename file and the
   * replyActionHandler and userDefinedActionHandler handlers.
   */
  static SingleSessionChatbot::SharedPtr
  createSingleSessionChatbotFromJsonModel(
      std::istream& model,
      Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler);

  /**
   * \param dictionaryModel             The data model to be loaded in the
   * chatbot.
   * \param interpreterModel              The data model describing the dialogs
   * enabled by the chatbot.
   * \param userDefinedActionHandler  The user implementation of the
   * userDefinedActionHandler that will be used
   * by the chatbot.
   * \return A MultiSessionChatbot.
   *
   * \brief Create a single session chatbot from an OpenIntent markup language,
   * a dictionary model and the
   * replyActionHandler and userDefinedActionHandler handlers.
   */
  static SingleSessionChatbot::SharedPtr createSingleSessionChatbotFromOIML(
      std::istream& dictionaryModel, std::istream& interpreterModel,
      Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler);

  /**
   * \param modelFilename             The data model file to be loaded in the
   * chatbot.
   * \param replyActionHandler        The user implementation of the
   * replyActionHandler that will be used by the
   * chatbot.
   * \param userDefinedActionHandler  The user implementation of the
   * userDefinedActionHandler that will be used
   * by the chatbot.
   * \return A MultiSessionChatbot.
   *
   * \brief Create a multi session chatbot with the datamodel contained in
   * modelFilename file and the
   * replyActionHandler and userDefinedActionHandler handlers.
   */
  template <typename SessionIdType>
  static typename MultiSessionChatbot<SessionIdType>::SharedPtr
  createMultiSessionChatbotFromJsonModel(
      std::istream& model,
      typename MultiSessionChatbot<SessionIdType>::UserDefinedActionHandler::
          SharedPtr userDefinedActionHandler);

  /**
   * \param modelFilename             The data model file to be loaded in the
   * chatbot.
   * \param oimlFilename              The data model file describing the dialogs
   * enabled by the chatbot.
   * \param replyActionHandler        The user implementation of the
   * replyActionHandler that will be used by the
   * chatbot.
   * \param userDefinedActionHandler  The user implementation of the
   * userDefinedActionHandler that will be used
   * by the chatbot.
   * \return A MultiSessionChatbot.
   *
   * \brief Create a multi session chatbot from OpenIntent markup language, a
   * dictionary model and with the
   * replyActionHandler and userDefinedActionHandler handlers.
   */
  template <typename SessionIdType>
  static typename MultiSessionChatbot<SessionIdType>::SharedPtr
  createMultiSessionChatbotFromOIML(
      std::istream& dictionaryModel, std::istream& interpreterModel,
      typename MultiSessionChatbot<SessionIdType>::UserDefinedActionHandler::
          SharedPtr userDefinedActionHandler);

 private:
  static bool loadFromJsonModel(std::istream& model,
                                ChatbotModel& chatbotModel);
  static bool loadFromOIML(std::istream& dictionaryModel,
                           std::istream& interpreterModel,
                           ChatbotModel& chatbotModel,
                           InterpreterFeedback& feedback);
};
}

#include "ChatbotFactory.inl.hpp"

#endif  // INTENT_CHATBOTFACTORY_HPP
