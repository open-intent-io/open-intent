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
#include "intent/intent_service/IntentService.hpp"
#include "intent/intent_service/SentenceTokenizer.hpp"

#include "intent/utils/Deserializer.hpp"
#include "intent/utils/Logger.hpp"

#include <fstream>

namespace intent {
IntentService::IntentService(const IntentServiceModel& intentServiceModel)
    : m_intentServiceModel(intentServiceModel) {}

std::stringstream logResult(IntentService::Result& result) {
  std::stringstream ss;
  if (result.found) {
    ss << "The intent \"" + result.intent.intentId + "\" has been found.";
  } else {
    ss << "No intent found.";
  }
  return ss;
}

IntentMatcher::IntentResult IntentService::resolveIntent(
    const std::string& input, const DictionaryModel& dictionaryModel,
    const IntentModel::IntentIndex& intentByIdIndex) const {
  INTENT_LOG_INFO() << "Look for intent in \"" + input + "\"";

  intent::Tokenizer::Tokens tokens;
  SentenceTokenizer sentenceTokenizer(dictionaryModel);
  sentenceTokenizer.tokenize(input, tokens);

  // Try to match entities
  intent::EntitiesMatcher entitiesMatcher;
  EntitiesMatcher::Variables variables =
      entitiesMatcher.match(tokens, dictionaryModel);

  IntentService::Result result =
      IntentMatcher::match(dictionaryModel, variables, intentByIdIndex);

  INTENT_LOG_TRACE() << "Result = " << result;
  INTENT_LOG_INFO() << logResult(result);

  return result;
}

IntentService::Result IntentService::evaluate(const std::string& input) const {
  return resolveIntent(input, *m_intentServiceModel.dictionaryModel,
                       m_intentServiceModel.intentModel->intentsByIntentId);
}

std::ostream& operator<<(std::ostream& os,
                         const IntentService::Result& result) {
  return os << "{ found: " << result.found << ", "
            << "intent: " << result.intent << " }";
}
}
