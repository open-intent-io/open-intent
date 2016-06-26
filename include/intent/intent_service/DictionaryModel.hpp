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
#ifndef INTENT_DICTIONARYMODEL_HPP
#define INTENT_DICTIONARYMODEL_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <memory>

#include "Term.hpp"
#include "TermIndex.hpp"

namespace intent {
/**
 * \brief Representation of a dictionary of entities with aliases.
 *
 * This dictionary is made of words bound to entities. An entity is a group of
 * words bound to a concept.
 * For instance, it can handle an entity @number that contains "one", "two",
 * "three", ... It can also be a
 * @car_brand entity with the following words "Ferrari", "Maserati", "Renault",
 * ...
 */
class DictionaryModel {
 public:
  typedef std::string Entity;

  typedef std::unordered_map<int, Entity> EntityByEntityIdIndex;
  typedef std::unordered_map<std::string, int> RegexToEntityIdIndex;
  typedef std::shared_ptr<DictionaryModel> SharedPtr;

  DictionaryModel();
  void initBuiltInEntities();

  /**
   * \brief The dictionary of terms and aliases.
   */
  TermIndex dictionary;

  /**
   * \brief A map of entities by ids.
   */
  EntityByEntityIdIndex entitiesByEntityId;

  /**
   * \brief A map of regular expressions by entity id.
   */
  RegexToEntityIdIndex regexesByEntityId;
};
}

#endif  // INTENT_DICTIONARYMODEL_HPP
