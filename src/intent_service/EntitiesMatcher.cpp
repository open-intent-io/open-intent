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
// Created by clement on 03/05/16.
//

#include "intent/intent_service/EntitiesMatcher.hpp"
#include "intent/utils/RegexMatcher.hpp"
#include <algorithm>

namespace intent {

int checkRegexes(const std::string& term,
                 DictionaryModel::RegexToEntityIdIndex regexes) {
  DictionaryModel::RegexToEntityIdIndex::iterator foundRegex =
      std::find_if(regexes.begin(), regexes.end(),
                   [&term](const std::pair<std::string, int>& regexToEntityId) {
                     return RegexMatcher::match(term, regexToEntityId.first);
                   });

  return (foundRegex != regexes.end()) ? foundRegex->second : -1;
}

EntitiesMatcher::Variables EntitiesMatcher::match(
    const Tokenizer::Tokens& tokens, const DictionaryModel& dictionaryModel) {
  Variables variables;
  std::vector<std::string>::const_iterator it = tokens.begin();
  std::vector<std::string>::const_iterator itEnd = tokens.end();

  while (it != itEnd) {
    int tokensPopped = 1;
    const std::vector<std::string> tokenBuffer(it, itEnd);
    Term term =
        dictionaryModel.dictionary.findTerm(*it, tokenBuffer, tokensPopped);
    Variable v;
    if (term.termId != -1) {
      v.text = *it;
      v.term = term.termId;
      v.entity = term.entityId;
      variables.push_back(v);
    } else {
      int entityId = checkRegexes(*it, dictionaryModel.regexesByEntityId);
      if (entityId != -1) {
        v.text = *it;
        v.term = -1;
        v.entity = entityId;
        variables.push_back(v);
      }
    }
    it += tokensPopped;
  }

  return variables;
}

std::ostream& operator<<(std::ostream& os,
                         const EntitiesMatcher::Variable& obj) {
  return os << "{ text: \"" << obj.text << "\", "
            << "term: " << obj.term << ", "
            << "entity: " << obj.entity << " }";
}
}
