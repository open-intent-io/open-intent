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
#include "intent/utils/Tokenizer.hpp"
#include "intent/utils/RegexMatcher.hpp"
#include "intent/utils/SingleCharacterDelimiterTokenizer.hpp"
#include "intent/utils/Logger.hpp"

#include <regex>

namespace intent {

Tokenizer::Tokenizer(const std::string& delimiters,
                     const std::vector<std::string>& regexpList)
    : m_delimiters(delimiters), m_regexpList(regexpList) {}

struct Split {
  std::string part;
  bool tokenizable;
};
typedef std::vector<Split> Splits;
bool operator==(const Split& s1, const Split& s2) {
  return s1.part == s2.part && s1.tokenizable == s2.tokenizable;
}

void splitMatch(const std::smatch& match, Splits& nextSplits) {
  Split s1, s2, s3;

  if (match.prefix().length() > 0) {
    s1.part = match.prefix();
    s1.tokenizable = true;
    nextSplits.push_back(s1);
  }

  s2.part = match[0];
  s2.tokenizable = false;
  nextSplits.push_back(s2);

  if (match.suffix().length() > 0) {
    s3.part = match.suffix();
    s3.tokenizable = true;
    nextSplits.push_back(s3);
  }
}

Splits splitRegexp(const std::string& message,
                   const std::vector<std::string>& regexpList) {
  Splits splits, nextSplits;
  Split split;
  split.part = message;
  split.tokenizable = true;
  nextSplits.push_back(split);

  while (splits != nextSplits) {
    bool splitted = false;
    splits = nextSplits;
    nextSplits.clear();

    Splits::const_iterator it = splits.begin();
    Splits::const_iterator itEnd = splits.end();
    for (; it != itEnd; ++it) {
      bool found_matching = false;
      const Split& split = *it;

      if (split.tokenizable && !regexpList.empty()) {
        for (const std::string& regexStr : regexpList) {
          std::regex expression(regexStr);
          std::smatch match;
          splitted = std::regex_search(split.part, match, expression);
          found_matching |= splitted;

          if (splitted) {
            splitMatch(match, nextSplits);
            break;
          }
        }
        if (!found_matching) {
          nextSplits.push_back(split);
        }
      } else {
        nextSplits.push_back(split);
      }
    }
  }

  return splits;
}

void tokenizeSplitsWithSingleCharacterDelimiter(const Splits& splits,
                                                const std::string& delimiters,
                                                Tokenizer::Tokens& tokens) {
  Splits::const_iterator it = splits.begin();
  Splits::const_iterator itEnd = splits.end();
  for (; it != itEnd; ++it) {
    const Split& split = *it;

    if (split.tokenizable) {
      std::vector<std::string> partTokens;
      SingleCharacterDelimiterTokenizer::tokenize(split.part, delimiters,
                                                  partTokens);
      tokens.insert(tokens.end(), partTokens.begin(), partTokens.end());
    } else {
      tokens.push_back(split.part);
    }
  }
}

template <typename T>
std::string join(const std::vector<T>& v) {
  std::stringstream ss;
  for (size_t i = 0; i < v.size(); ++i) {
    if (i != 0) ss << ",";
    ss << v[i];
  }
  return ss.str();
}

std::string logTokenization(const std::string& message,
                            const Tokenizer::Tokens& tokens) {
  std::stringstream ss;
  ss << "Tokenization of \"" << message << "\" gives \"" << join(tokens)
     << "\"";
  return ss.str();
}

void Tokenizer::tokenize(const std::string& message, Tokens& tokens) {
  Splits splits = splitRegexp(message, m_regexpList);
  tokenizeSplitsWithSingleCharacterDelimiter(splits, m_delimiters, tokens);
  INTENT_LOG_TRACE() << logTokenization(message, tokens);
}
}
