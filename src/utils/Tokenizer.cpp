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

#include "intent/utils/Tokenizer.hpp"
#include "intent/utils/RegexMatcher.hpp"

#include <boost/algorithm/string.hpp>

namespace intent {
void Tokenizer::tokenize(const std::string& input,
                         const std::string& delimiters, Tokens& tokens) {
  std::string sentence = RegexMatcher::padAroundRegexMarkersInSentence(input);

  boost::split(tokens, sentence, boost::is_any_of(delimiters));

  // Remove empty characters
  Tokens::iterator itEnd = std::remove(tokens.begin(), tokens.end(), "");
  tokens.erase(itEnd, tokens.end());
}

namespace {

struct is_any_of_chars {
  is_any_of_chars(const std::vector<char>& delimiters)
      : m_delimiters(delimiters) {}

  bool operator()(const char& c) const {
    return std::find_if(m_delimiters.begin(), m_delimiters.end(),
                        [&c](const char& delimiter) {
                          return c == delimiter;
                        }) != m_delimiters.end();
  }

  const std::vector<char>& m_delimiters;
};
}

void Tokenizer::tokenize(const std::string& input,
                         const std::vector<char>& delimiters, Tokens& tokens) {
  boost::split(tokens, input, is_any_of_chars(delimiters));

  // Remove empty characters
  Tokens::iterator itEnd = std::remove(tokens.begin(), tokens.end(), "");
  tokens.erase(itEnd, tokens.end());
}
}
