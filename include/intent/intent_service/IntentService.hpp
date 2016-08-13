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
#ifndef INTENT_INTENTSERVICE_HPP
#define INTENT_INTENTSERVICE_HPP

#include <memory>
#include <string>

#include "intent/intent_service/EntitiesMatcher.hpp"
#include "intent/intent_service/IntentMatcher.hpp"
#include "IntentServiceModel.hpp"

namespace intent {

/**
 * \brief Public interface that returns a user intent given a dictionary and an
 * intent service data model.
 */
class IntentService {
 public:
  typedef IntentMatcher::IntentResult Result;
  typedef IntentMatcher::EntityMatch EntityMatch;
  typedef IntentMatcher::EntityMatches EntityMatches;

  /**
   * \param intentServiceModel    The model used to find intents in sentences.
   */
  IntentService(const IntentServiceModel& intentServiceModel);

  /**
   * \brief Try to find a user intent and returns the result.
   */
  Result evaluate(const std::string& input) const;

  const IntentServiceModel& getIntentServiceModel() const {
    return m_intentServiceModel;
  }

 protected:
  IntentMatcher::IntentResult resolveIntent(
      const std::string& input, const DictionaryModel& dictionaryModel,
      const IntentModel::IntentIndex& intentByIdIndex) const;

  IntentServiceModel m_intentServiceModel;
};

std::ostream& operator<<(std::ostream& os, const IntentService::Result& result);
}

#endif  // INTENT_INTENTSERVICE_HPP
