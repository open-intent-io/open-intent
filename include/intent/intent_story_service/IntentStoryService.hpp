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
#ifndef INTENT_INTENTSTORYSERVICE_HPP
#define INTENT_INTENTSTORYSERVICE_HPP

#include "intent/intent_service/IntentService.hpp"

#include "intent/intent_service/DictionaryModel.hpp"
#include "intent/intent_service/IntentService.hpp"
#include "IntentStoryModel.hpp"
#include "IntentStoryServiceModel.hpp"

namespace intent {
/**
 * \brief The service uses a finite state automaton to match user successive
 * intents. We call it an intent story.
 */
class IntentStoryService : protected IntentService {
 public:
  typedef IntentMatcher::Intent Intent;
  typedef IntentMatcher::EntityMatch EntityMatch;
  typedef IntentMatcher::EntityMatches EntityMatches;

  /**
   * \brief The result of one transition of the finite state automaton after
   * evaluating a single user intent.
   */
  struct Result {
    Result() : found(false) {}

    bool found;
    std::string actionId;
    std::string nextStateId;
    Intent intent;

    bool operator==(const Result& that) const {
      return this->found == that.found && this->actionId == that.actionId &&
             this->intent == that.intent &&
             this->nextStateId == that.nextStateId;
    }
  };

  /**
   * \param intentStoryServiceModel   The intent story service data model
   * containing the finite state automaton.
   */
  IntentStoryService(const IntentStoryServiceModel& intentStoryServiceModel);

  /**
   * \brief Evaluate a user intent and try to find a transition to move on in
   * the following state.
   *
   * \param stateId   The state ID to evaluate the intent from.
   * \param message   The user intent.
   * \return The result of the intent matching.
   */
  Result evaluate(const std::string& stateId, const std::string& message) const;

  inline IntentStoryServiceModel getIntentStoryServiceModel() const {
    IntentStoryServiceModel intentStoryServiceModel;
    intentStoryServiceModel.intentServiceModel = getIntentServiceModel();
    intentStoryServiceModel.intentStoryModel = m_intentStoryModel;
    return intentStoryServiceModel;
  }

 private:
  IntentStoryModel::SharedPtr m_intentStoryModel;
};

std::ostream& operator<<(std::ostream& os,
                         const IntentStoryService::Result& result);
}

#endif  // INTENT_INTENTSTORYSERVICE_HPP
