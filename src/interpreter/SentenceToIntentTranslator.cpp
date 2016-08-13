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
#include "intent/interpreter/SentenceToIntentTranslator.hpp"

#include "intent/intent_service/EntitiesMatcher.hpp"
#include "intent/intent_service/IntentEncoder.hpp"
#include "intent/intent_service/IntentModel.hpp"
#include "intent/intent_service/SentenceTokenizer.hpp"

#include "intent/utils/Logger.hpp"
#include "intent/utils/Tokenizer.hpp"

namespace intent {
typedef SentenceToIntentTranslator::IndexType IndexType;
typedef SentenceToIntentTranslator::Intent Intent;

namespace {

std::vector<int> extractEntities(const EntitiesMatcher::Variables& variables) {
  std::vector<int> entities;
  std::transform(
      variables.begin(), variables.end(), std::back_inserter(entities),
      [](const EntitiesMatcher::Variable& var) { return var.entity; });
  return entities;
}

void completeVariableNames(const std::vector<int>& entities,
                           const DictionaryModel& dictionaryModel,
                           IntentModel::EntityToNames& entityToNames) {
  // default names for variables
  std::unordered_map<int, int> entityIdToCounter;
  std::for_each(
      entities.begin(), entities.end(),
      [&entityToNames, &dictionaryModel,
       &entityIdToCounter](const int& entity) {
        DictionaryModel::EntityByEntityIdIndex::const_iterator it =
            dictionaryModel.entitiesByEntityId.find(entity);
        std::string entityName;
        if (it != dictionaryModel.entitiesByEntityId.end())
          entityName = it->second;

        int& counter = entityIdToCounter[entity];
        entityToNames[entityName].push(entityName + std::to_string(counter));
        ++counter;
      });
}
}

std::string logEntities(const std::vector<int>& entityIds,
                        const DictionaryModel& dictionaryModel) {
  std::vector<std::string> entities;
  std::stringstream ss;

  std::transform(entityIds.begin(), entityIds.end(),
                 std::back_inserter(entities), [&dictionaryModel](int entity) {
                   DictionaryModel::EntityByEntityIdIndex::const_iterator it =
                       dictionaryModel.entitiesByEntityId.find(entity);
                   if (it != dictionaryModel.entitiesByEntityId.end()) {
                     return it->second;
                   }
                   return std::string("");
                 });

  std::copy(entities.begin(), entities.end(),
            std::ostream_iterator<std::string>(ss, ", "));

  std::string entitiesString = ss.str();
  return entitiesString.substr(0, entitiesString.size() - 2);
}

std::pair<IndexType, Intent> SentenceToIntentTranslator::translate(
    const std::string& sentence, const DictionaryModel& dictionaryModel) {
  IntentModel::Intent intent;

  std::vector<std::string> tokens;
  SentenceTokenizer sentenceTokenizer(dictionaryModel);
  sentenceTokenizer.tokenize(sentence, tokens);
  std::vector<int> entities =
      extractEntities(EntitiesMatcher::match(tokens, dictionaryModel));
  intent.entities = entities;

  IntentModel::EntityToNames entityToVariableNames;
  completeVariableNames(entities, dictionaryModel, entityToVariableNames);
  intent.entityToVariableNames = entityToVariableNames;

  LOG_INFO() << "Translate sentence \"" + sentence +
                    "\" into intent with following entities [" +
                    logEntities(entities, dictionaryModel) + "].";

  std::string intentId = IntentEncoder::encode(entities);
  intent.intentId = intentId;
  intent.example = sentence;
  return std::pair<std::string, IntentModel::Intent>(intentId, intent);
}
}
