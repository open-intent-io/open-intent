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
#ifndef INTENT_INTENTMODEL_HPP
#define INTENT_INTENTMODEL_HPP

#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>

namespace intent {
/**
 * \brief The data model used to find a user intent
 *
 * This data model has two parts. A dictionary of entities and model of intents
 * that are combination of entities.
 */
class IntentModel {
 public:
  typedef std::map<std::string, std::queue<std::string>> EntityToNames;

  /**
   * \brief Representation of an intent.
   */
  struct Intent {
    std::string intentId;
    std::vector<int> entities;
    EntityToNames entityToVariableNames;
    std::string example;
  };

  typedef std::string IndexType;
  typedef std::map<IndexType, Intent> IntentIndex;
  typedef std::shared_ptr<IntentModel> SharedPtr;

  /**
   * Index of intents by intent ID.
   */
  IntentIndex intentsByIntentId;
};

std::ostream& operator<<(std::ostream& os, const IntentModel::Intent& intent);
}

#endif  // INTENT_INTENTMODEL_HPP
