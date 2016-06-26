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
#ifndef INTENT_TERM_HPP
#define INTENT_TERM_HPP

#include <ostream>

#include <vector>
#include <string>

namespace intent {
/**
 * \brief A Term is a word or a group of words meaningful in an intent.
 *
 * A set of aliases can be attached to a term. If any of those aliases is found
 * in the user input,
 * the word will be replaced by the term and match the corresponding entity.
 */
class Term {
 public:
  typedef std::vector<std::string> AliasIndex;

  std::string term;
  std::string lowerCaseTerm;
  AliasIndex alias;
  int entityId;
  int termId;

  Term() : entityId(-1), termId(-1) {}

  bool operator==(const Term& that) const {
    return this->term == that.term && this->alias == that.alias &&
           this->entityId == that.entityId &&
           this->lowerCaseTerm == that.lowerCaseTerm;
  }
};

std::ostream& operator<<(std::ostream& os, const Term& obj);
}

#endif  // INTENT_TERM_HPP
