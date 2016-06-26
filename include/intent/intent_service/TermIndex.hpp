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
#ifndef INTENT_TERMINDEX_HPP
#define INTENT_TERMINDEX_HPP

#include "Term.hpp"
#include <unordered_map>
#include <set>

namespace intent {
typedef std::unordered_map<std::string, std::set<int>> TrigramIndex;

/**
 * \brief Index of terms using trigrams to allow error correction during the
 * matching.
 */
class TermIndex {
 public:
  /**
   * \brief pushTerm      pushes a term into the index, so it can be later found
   * by fuzzy matching
   * \param term          the term to push
   */
  void pushTerm(const Term& term);

  /**
   * \brief findTerm      finds a term in the index if it is constituted by one
   * word
   * \param token         the word to match with an alias of the term
   * \return foundTerm
   */
  Term findTerm(const std::string& token) const;

  /**
   * \brief findTerm      finds a term in the index it can be constituted of
   * multiple consecutive words
   * \param token         the word to match with an alias of the term
   * \param buffer        the rest of the words of the sentence
   * \param tokensPopped  the number of additional words of the sentence that
   * are used to match the term
   * \return foundTerm
   */
  Term findTerm(const std::string& token,
                const std::vector<std::string>& buffer,
                int& tokensPopped) const;

  /**
   * \brief findTerm      finds the term by the term id in the index
   * \param termId        the term id
   * \return foundTerm
   */
  Term findTerm(const int termId) const;

  /**
   * \brief getTermSet    gets all the terms that contain the trigram (internal
   * implementation of the index)
   * \param trigram
   * \param termIds       the term ids returned by the query
   */
  void getTermSet(const std::string& trigram, std::set<int>& termIds) const;

  size_t size() const;

  size_t index_size() const;

 private:
  TrigramIndex index;
  std::unordered_map<int, Term> dictionary;
};
}

#endif  // INTENT_TERMINDEX_HPP
