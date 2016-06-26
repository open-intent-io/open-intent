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
#ifndef INTENT_DESERIALIZER_HPP
#define INTENT_DESERIALIZER_HPP

#include "json.hpp"

#include "intent/chatbot/ChatbotActionModel.hpp"
#include "intent/chatbot/ChatbotModel.hpp"
#include "intent/intent_service/DictionaryModel.hpp"
#include "intent/intent_service/IntentModel.hpp"
#include "intent/intent_story_service/IntentStoryModel.hpp"
#include "intent/utils/Exception.hpp"

namespace intent {
/**
 * \brief Deserializer for every JSON intent datamodels.
 */
class Deserializer {
 private:
  template <typename T>
  struct Type2Type {};

 public:
  static const std::string REGEX_SYMBOL;

  /**
   * \brief Deserialize the datamodel given as template parameter from istream
   */
  template <class T>
  T deserialize(std::istream& is) {
    return deserialize(is, Type2Type<T>());
  }

  /**
   * Deserialize the datamodel given as template parameter from a JSON object
   */
  template <class T>
  T deserialize(const nlohmann::json& input) {
    return deserialize(input, Type2Type<T>());
  }

 private:
  DictionaryModel::SharedPtr deserialize(std::istream& is,
                                         Type2Type<DictionaryModel::SharedPtr>);

  DictionaryModel::SharedPtr deserialize(const nlohmann::json& input,
                                         Type2Type<DictionaryModel::SharedPtr>);

  /**
   * \brief Deserialize an IntentServiceModel from an input stream.
   */
  IntentServiceModel deserialize(std::istream& is,
                                 Type2Type<IntentServiceModel>);

  /**
   * \brief Deserialize an IntentServiceModel from a JSON object.
   */
  IntentServiceModel deserialize(const nlohmann::json& input,
                                 Type2Type<IntentServiceModel>);

  IntentStoryServiceModel deserialize(std::istream& is,
                                      Type2Type<IntentStoryServiceModel>);
  IntentStoryServiceModel deserialize(const nlohmann::json& input,
                                      Type2Type<IntentStoryServiceModel>);

  ChatbotModel deserialize(std::istream& is, Type2Type<ChatbotModel>);
  ChatbotModel deserialize(const nlohmann::json& input,
                           Type2Type<ChatbotModel>);
};

/**
 * \brief Exception that can be thrown during datamodel deserialization.
 */
class DeserializerException : public Exception {
 public:
  /**
   * \brief Constructs an exception with an error message.
   *
   * \param error The error message
   */
  DeserializerException(const std::string& error) : Exception(error) {}
};
}

#endif
