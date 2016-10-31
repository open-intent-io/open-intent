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
// Created by clement on 12/05/16.
//

#include "intent/intent_service/IntentMatcher.hpp"
#include "intent/utils/RegexMatcher.hpp"
#include "intent/intent_service/IntentEncoder.hpp"

#include <algorithm>
#include <iterator>
#include <sstream>

namespace intent {
namespace {

void extractEntityIds(const EntitiesMatcher::Variables& variables,
                      std::vector<int>& entityIds) {
  std::for_each(variables.begin(), variables.end(),
                [&entityIds](const EntitiesMatcher::Variable& v) {
                  entityIds.push_back(v.entity);
                });
}

void matchIntents(
    const IntentModel::IntentIndex& intentByIdIndex,
    const std::vector<int>& entityIds,
    std::vector<std::pair<IntentModel::IndexType, IntentModel::Intent>>&
        foundIntents) {
  std::copy_if(
      intentByIdIndex.begin(), intentByIdIndex.end(),
      std::back_inserter(foundIntents),
      [&entityIds](
          const std::pair<IntentModel::IndexType, IntentModel::Intent>& p) {
        return RegexMatcher::match(IntentEncoder::encode(entityIds),
                                   "^" + IntentEncoder::encode(p.second) + "$");
      });
}

void completeMatch(IntentMatcher::EntityMatch& entityMatch,
                   const EntitiesMatcher::Variable& variable,
                   const DictionaryModel& dico) {
  entityMatch.term = dico.dictionary.findTerm(variable.term).term;
  entityMatch.text = variable.text;
}

void fillMatchEntity(IntentMatcher::EntityMatch& entityMatch,
                     const EntitiesMatcher::Variable& variable,
                     const DictionaryModel& dico) {
  DictionaryModel::EntityByEntityIdIndex::const_iterator entityIt =
      dico.entitiesByEntityId.find(variable.entity);
  if (entityIt != dico.entitiesByEntityId.end())
    entityMatch.entity = entityIt->second;
}

struct MatchNamer {
  MatchNamer(IntentModel::EntityToNames& entityToNames)
      : m_entityToNames(entityToNames) {}

  void operator()(IntentMatcher::EntityMatch& match) {
    std::queue<std::string>& variableNames = m_entityToNames[match.entity];
    if (!variableNames.empty()) {
      match.name = variableNames.front();
      variableNames.pop();
    } else {
      match.name = match.entity + std::to_string(entityCounter[match.entity]);
      entityCounter[match.entity]++;
    }
  }

  std::map<std::string, int> entityCounter;
  IntentModel::EntityToNames& m_entityToNames;
};

}  // anonymous

void fillIntentResult(const DictionaryModel& dictionaryModel,
                      IntentMatcher::IntentResult& intentResult,
                      const EntitiesMatcher::Variables& variables,
                      IntentModel::EntityToNames& entityToVariableNames) {
  MatchNamer nameMatch(entityToVariableNames);
  std::transform(
      variables.begin(), variables.end(),
      std::back_inserter(intentResult.intent.entityMatches),
      [&dictionaryModel, &nameMatch](const EntitiesMatcher::Variable& v) {
        IntentMatcher::EntityMatch match;
        fillMatchEntity(match, v, dictionaryModel);
        nameMatch(match);
        completeMatch(match, v, dictionaryModel);
        return match;
      });
}

IntentMatcher::IntentResult IntentMatcher::match(
    const DictionaryModel& dictionaryModel,
    const EntitiesMatcher::Variables& variables,
    const IntentModel::IntentIndex& intentByIdIndex) {
  IntentMatcher::IntentResult intentResult;

  std::vector<int> entityIds;
  extractEntityIds(variables, entityIds);

  std::vector<std::pair<IntentModel::IndexType, IntentModel::Intent>>
      foundIntents;
  matchIntents(intentByIdIndex, entityIds, foundIntents);

  if (foundIntents.size() == 1) {
    intentResult.found = true;
    intentResult.intent.intentId = foundIntents[0].first;

    IntentModel::EntityToNames entityToVariableNames =
        foundIntents[0].second.entityToVariableNames;
    fillIntentResult(dictionaryModel, intentResult, variables,
                     entityToVariableNames);
  }

  return intentResult;
}

IntentMatcher::Intent IntentMatcher::buildFullMatchIntent(
    const std::string& message) {
  Intent intent;
  EntityMatch fullMatch(message, message, "", "_full");
  intent.entityMatches.push_back(fullMatch);
  return intent;
}

std::ostream& operator<<(std::ostream& os,
                         const IntentMatcher::Intent& intent) {
  return os << "{ intentId: " << intent.intentId << ", "
            << "variables: " << intent.entityMatches << " }";
}

std::ostream& operator<<(std::ostream& os,
                         const IntentMatcher::EntityMatches& entityMatches) {
  std::stringstream ss;
  std::copy(entityMatches.begin(), entityMatches.end(),
            std::ostream_iterator<IntentMatcher::EntityMatch>(ss, ", "));
  return os << ss.str();
}

std::ostream& operator<<(std::ostream& os,
                         const IntentMatcher::EntityMatch& entityMatch) {
  return os << "{ entity: " << entityMatch.entity << ", "
            << "term: " << entityMatch.term << ", "
            << "text: " << entityMatch.text << " }";
}
}
