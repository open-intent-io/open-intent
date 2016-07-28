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
#include "intent/intent_service/DictionaryModel.hpp"
#include "intent/utils/Levenshtein.hpp"

#include <algorithm>
#include "intent/intent_service/DictionaryModel.hpp"
#include "intent/intent_service/TermIndex.hpp"
#include "intent/utils/Levenshtein.hpp"
#include "intent/utils/SingleCharacterDelimiterTokenizer.hpp"
#include "intent/utils/Tokenizer.hpp"
#include "intent/utils/TrigramHelper.hpp"
#include <unordered_map>
#include <cassert>

namespace intent {

namespace {

struct pushTermIdForTrigram {
  pushTermIdForTrigram(TrigramIndex& trigramIndex, int termId)
      : trigramIndex(trigramIndex), termId(termId) {}

  void operator()(const std::string& trigram) {
    trigramIndex[trigram].insert(termId);
  }

  TrigramIndex& trigramIndex;
  int termId;
};

struct evaluateTrigram {
  evaluateTrigram(const TrigramIndex& trigramIndex,
                  std::unordered_map<int, int>& scores)
      : trigramIndex(trigramIndex), scores(scores) {}

  void operator()(const std::string& trigram) {
    TrigramIndex::const_iterator it = trigramIndex.find(trigram);
    if (it != trigramIndex.end()) {
      std::set<int> termIds = it->second;
      for (std::set<int>::const_iterator termId = termIds.begin();
           termId != termIds.end(); ++termId) {
        ++scores[*termId];
      }
    }
  }

  const TrigramIndex& trigramIndex;
  std::unordered_map<int, int>& scores;
};

struct findMaxScore {
  std::pair<int, std::set<int>> operator()(
      std::pair<int, std::set<int>>& maxScoreTerms,
      const std::pair<int, int>& score) {
    if (score.second > maxScoreTerms.first) {
      maxScoreTerms.second.clear();
      maxScoreTerms.second.insert(score.first);
      maxScoreTerms.first = score.second;
    } else if (score.second == maxScoreTerms.first) {
      maxScoreTerms.second.insert(score.first);
    }
    return maxScoreTerms;
  }
};

struct pushAlias {
  pushAlias(TrigramIndex& trigramIndex, const int termId)
      : trigramIndex(trigramIndex), termId(termId) {}

  void operator()(const std::string& alias) {
    std::vector<std::string> trigrams;
    TrigramHelper::generateTrigrams(alias, trigrams);
    std::for_each(trigrams.begin(), trigrams.end(),
                  pushTermIdForTrigram(trigramIndex, termId));
  }

  TrigramIndex& trigramIndex;
  const int termId;
};

struct TermFinderForId {
  TermFinderForId(const std::unordered_map<int, Term>& index)
      : m_index(index) {}

  Term operator()(const int id) {
    std::unordered_map<int, Term>::const_iterator potentialTerm =
        m_index.find(id);
    Term foundTerm;
    if (potentialTerm != m_index.end()) foundTerm = potentialTerm->second;
    return foundTerm;
  }

  const std::unordered_map<int, Term>& m_index;
};

bool isInvalidTerm(const Term& term) {
  return term.termId == -1 || term.entityId == -1;
}

typedef std::unordered_map<std::string, std::string> AliasesToBuffered;

struct ZipWithBuffer {
  ZipWithBuffer(const std::vector<std::string>& buffer) : m_buffer(buffer) {}

  AliasesToBuffered& operator()(AliasesToBuffered& aliases, const Term& term) {
    Term::AliasIndex termAliases = term.alias;
    aliases = operator()(aliases, term.lowerCaseTerm);
    aliases = std::accumulate(termAliases.begin(), termAliases.end(), aliases,
                              ZipWithBuffer(m_buffer));
    return aliases;
  }

  AliasesToBuffered& operator()(AliasesToBuffered& aliases,
                                const std::string& alias) {
    std::vector<std::string>::const_iterator bufferIt = m_buffer.begin();
    std::vector<std::string>::const_iterator bufferEnd = m_buffer.end();

    std::vector<std::string> tokens;
    SingleCharacterDelimiterTokenizer::tokenize(alias, " ", tokens);

    std::vector<std::string>::const_iterator tokenIt = tokens.begin();
    for (; tokenIt != tokens.end() && bufferIt != m_buffer.end();
         ++tokenIt, ++bufferIt) {
      std::string lowered = *bufferIt;
      std::transform(lowered.begin(), lowered.end(), lowered.begin(),
                     ::tolower);
      aliases[alias] += lowered + " ";
    }
    return aliases;
  }

  const std::vector<std::string>& m_buffer;
};

typedef std::unordered_map<std::string, Term> AliasToTerm;

struct IndexAliases {
  AliasToTerm& operator()(AliasToTerm& aliasToTerm, const Term& term) {
    aliasToTerm[term.lowerCaseTerm] = term;
    std::for_each(term.alias.begin(), term.alias.end(),
                  [&aliasToTerm, &term](const std::string& alias) {
                    aliasToTerm[alias] = term;
                  });
    return aliasToTerm;
  }
};

typedef std::pair<std::string, int> BestLevenshteinMatch;

struct LevenshteinMinimizer {
  BestLevenshteinMatch operator()(
      const BestLevenshteinMatch& bestAliasMatch,
      std::pair<std::string, std::string> aliasToCounterpart) const {
    int distance = Levenshtein::distance(aliasToCounterpart.first,
                                         aliasToCounterpart.second);
    if (distance < bestAliasMatch.second) {
      return BestLevenshteinMatch(aliasToCounterpart.first, distance);
    }
    return bestAliasMatch;
  }
};

}  // anonymous

void TermIndex::pushTerm(const Term& term) {
  Term updatedTerm = term;
  // Lower the term
  std::transform(updatedTerm.term.begin(), updatedTerm.term.end(),
                 std::back_inserter(updatedTerm.lowerCaseTerm), ::tolower);

  // Lower the aliases
  std::for_each(updatedTerm.alias.begin(), updatedTerm.alias.end(),
                [](std::string& alias) {
                  std::transform(alias.begin(), alias.end(), alias.begin(),
                                 ::tolower);
                });

  pushAlias pusher(index, updatedTerm.termId);

  pusher(updatedTerm.term);
  std::for_each(updatedTerm.alias.begin(), updatedTerm.alias.end(), pusher);

  dictionary[updatedTerm.termId] = updatedTerm;
}

Term TermIndex::findTerm(const std::string& token,
                         const std::vector<std::string>& buffer,
                         int& tokensPopped) const {
  std::string lowered_token;
  std::transform(token.begin(), token.end(), std::back_inserter(lowered_token),
                 ::tolower);

  std::vector<std::string> trigrams;
  std::unordered_map<int, int> termIdToScore;
  TrigramHelper::generateTrigrams(lowered_token, trigrams);

  std::for_each(trigrams.begin(), trigrams.end(),
                evaluateTrigram(index, termIdToScore));

  std::pair<int, std::set<int>> maxScoreTerms;
  maxScoreTerms = std::accumulate(termIdToScore.begin(), termIdToScore.end(),
                                  maxScoreTerms, findMaxScore());

  std::set<int> foundTermIds = maxScoreTerms.second;
  std::vector<Term> foundTerms;

  // if the score is too low, we shouldn't accept the match
  int threshold = std::min((int)trigrams.size() / 2, 2);
  if (maxScoreTerms.first > threshold && !foundTermIds.empty()) {
    // we find the terms by termId
    std::transform(foundTermIds.begin(), foundTermIds.end(),
                   std::back_inserter(foundTerms), TermFinderForId(dictionary));

    // we remove invalid tokens
    std::vector<Term>::const_iterator invalidTermsStart =
        std::remove_if(foundTerms.begin(), foundTerms.end(), isInvalidTerm);
    foundTerms.erase(invalidTermsStart, foundTerms.end());
  }

  Term term;
  if (!foundTerms.empty()) {
    // we align each alias with the same number of tokens of the sentence
    AliasesToBuffered aliases;
    aliases = std::accumulate(foundTerms.begin(), foundTerms.end(), aliases,
                              ZipWithBuffer(buffer));

    // we index again the aliases to retrieve the term once done
    std::unordered_map<std::string, Term> aliasesToTerm;
    aliasesToTerm = std::accumulate(foundTerms.begin(), foundTerms.end(),
                                    aliasesToTerm, IndexAliases());

    // we calculate the alias which has best levenshtein distance with its multi
    // word counterpart
    BestLevenshteinMatch initPair(std::string(), 0xFFFF);
    BestLevenshteinMatch bestAlias = std::accumulate(
        aliases.begin(), aliases.end(), initPair, LevenshteinMinimizer());

    // the distance must be small enough in the end
    if (bestAlias.second <= 2) {
      term = aliasesToTerm[bestAlias.first];

      std::vector<std::string> tokens;
      SingleCharacterDelimiterTokenizer::tokenize(bestAlias.first, " ", tokens);
      tokensPopped = (int)tokens.size();
      assert(tokensPopped > 0);
    }
  }

  return term;
}

Term TermIndex::findTerm(const std::string& token) const {
  const std::vector<std::string> buffer({token});
  int tokensPopped = 1;
  return findTerm(token, buffer, tokensPopped);
}

Term TermIndex::findTerm(const int termId) const {
  Term term;
  std::unordered_map<int, Term>::const_iterator it = dictionary.find(termId);
  if (it != dictionary.end()) term = it->second;
  return term;
}

size_t TermIndex::size() const { return dictionary.size(); }

size_t TermIndex::index_size() const { return index.size(); }

void TermIndex::getTermSet(const std::string& trigram,
                           std::set<int>& termIds) const {
  TrigramIndex::const_iterator it = index.find(trigram);
  if (it != index.end()) termIds = it->second;
}
}
