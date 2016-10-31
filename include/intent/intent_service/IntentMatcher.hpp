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
#ifndef INTENT_INTENTMATCHER_HPP
#define INTENT_INTENTMATCHER_HPP

#include <string>

#include "intent/intent_service/EntitiesMatcher.hpp"
#include "IntentModel.hpp"

namespace intent {
/**
 * \brief Matcher that returns the intent found in a list of entities.
 */
class IntentMatcher {
 public:
  typedef std::string IndexType;
  typedef EntitiesMatcher::Variable Variable;
  typedef EntitiesMatcher::Variables Variables;
  typedef IntentModel::IntentIndex IntentIndex;

  /**
   * \brief The entity match.
   */
  struct EntityMatch {
    std::string text;
    std::string term;
    std::string entity;
    std::string name;

    EntityMatch() {}

    EntityMatch(const std::string& text, const std::string& term,
                const std::string& entity, const std::string& name)
        : text(text), term(term), entity(entity), name(name) {}

    bool operator==(const EntityMatch& that) const {
      return this->text == that.text && this->term == that.term &&
             this->entity == that.entity;
    }
  };

  typedef std::vector<EntityMatch> EntityMatches;

  /**
   * \brief The representation of an intent.
   */
  struct Intent {
    IndexType intentId;
    EntityMatches entityMatches;

    bool operator==(const Intent& that) const {
      return this->intentId == that.intentId &&
             this->entityMatches == that.entityMatches;
    }
  };

  /**
   * \brief The result returned by the matcher
   *
   * This result contains whether the matcher found an intent and an intent.
   * If no intent has been found, the intent will by initialized with default
   * values and found will be set to true.
   */
  struct IntentResult {
    IntentResult() : found(false) {}

    bool found;
    Intent intent;

    bool operator==(const IntentResult& that) const {
      return this->found == that.found && this->intent == that.intent;
    }
  };

  /**
   * Matches the intent contained in the index and return the matching variables
   * as a result.
   */
  static IntentResult match(const DictionaryModel& dictionaryModel,
                            const Variables& variables,
                            const IntentIndex& intentByIdIndex);

  /**
   * Build full match intent, with one matching entity, the full user message
   *
   */

  static Intent buildFullMatchIntent(const std::string& message);
};

std::ostream& operator<<(std::ostream& os, const IntentMatcher::Intent& intent);
std::ostream& operator<<(std::ostream& os,
                         const IntentMatcher::EntityMatches& entityMatches);
std::ostream& operator<<(std::ostream& os,
                         const IntentMatcher::EntityMatch& entityMatch);
}

#endif  // INTENT_INTENTMATCHER_HPP
