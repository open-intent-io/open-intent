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
//
// Created by clement on 14/05/16.
//

#include "intent/chatbot/ChatbotFactory.hpp"

#include "intent/interpreter/Interpreter.hpp"
#include "intent/utils/Deserializer.hpp"

#include <fstream>

namespace intent {
Chatbot::SharedPtr ChatbotFactory::createChatbotFromJsonModel(
    std::istream& model) {
  ChatbotModel chatbotModel;
  intent::Chatbot::SharedPtr chatbot;
  if (ChatbotFactory::loadFromJsonModel(model, chatbotModel)) {
    chatbot.reset(new intent::Chatbot(chatbotModel));
  }
  return chatbot;
}

Chatbot::SharedPtr ChatbotFactory::createChatbotFromOIML(
    std::istream& dictionaryModel, std::istream& interpreterModel,
    InterpreterFeedback& feedback) {
  ChatbotModel chatbotModel;
  intent::Chatbot::SharedPtr chatbot;
  if (ChatbotFactory::loadFromOIML(dictionaryModel, interpreterModel,
                                   chatbotModel, feedback)) {
    chatbot.reset(new intent::Chatbot(chatbotModel));
  }
  return chatbot;
}

SingleSessionChatbot::SharedPtr
ChatbotFactory::createSingleSessionChatbotFromJsonModel(
    std::istream& model,
    Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler) {
  ChatbotModel chatbotModel;
  intent::SingleSessionChatbot::SharedPtr chatbot;
  if (ChatbotFactory::loadFromJsonModel(model, chatbotModel)) {
    chatbot.reset(new intent::SingleSessionChatbot(chatbotModel,
                                                   userDefinedActionHandler));
  }
  return chatbot;
}

SingleSessionChatbot::SharedPtr
ChatbotFactory::createSingleSessionChatbotFromOIML(
    std::istream& dictionaryModel, std::istream& interpreterModel,
    Chatbot::UserDefinedActionHandler::SharedPtr userDefinedActionHandler) {
  ChatbotModel chatbotModel;
  intent::SingleSessionChatbot::SharedPtr chatbot;
  InterpreterFeedback feedback;
  if (ChatbotFactory::loadFromOIML(dictionaryModel, interpreterModel,
                                   chatbotModel, feedback)) {
    chatbot.reset(new intent::SingleSessionChatbot(chatbotModel,
                                                   userDefinedActionHandler));
  }
  return chatbot;
}

bool ChatbotFactory::loadFromJsonModel(std::istream& model,
                                       ChatbotModel& chatbotModel) {
  bool loaded = false;
  if (model.good()) {
    intent::Deserializer deserializer;
    chatbotModel = deserializer.deserialize<ChatbotModel>(model);
    loaded = true;
  }

  return loaded;
}

bool ChatbotFactory::loadFromOIML(std::istream& dictionaryModel,
                                  std::istream& interpreterModel,
                                  ChatbotModel& chatbotModel,
                                  InterpreterFeedback& feedback) {
  bool loaded = false;
  if (dictionaryModel.good() && interpreterModel.good()) {
    intent::Deserializer deserializer;
    chatbotModel.intentStoryServiceModel.intentServiceModel.dictionaryModel =
        deserializer.deserialize<DictionaryModel::SharedPtr>(dictionaryModel);

    std::istreambuf_iterator<char> eos;
    std::string content(std::istreambuf_iterator<char>(interpreterModel), eos);

    chatbotModel = Interpreter::build(
        content,
        chatbotModel.intentStoryServiceModel.intentServiceModel.dictionaryModel,
        feedback);

    loaded = true;
  }

  return loaded;
}
}
