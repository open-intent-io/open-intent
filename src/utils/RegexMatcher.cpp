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
#include <boost/algorithm/string.hpp>

#include <regex>
#include <stdlib.h>
#include <string>
#include <iostream>

#include "intent/utils/RegexMatcher.hpp"

using namespace boost;

namespace intent {

const std::string RegexMatcher::REGEX_MARKERS("[]()|*");

std::string RegexMatcher::padAroundRegexMarkersInSentence(
    const std::string& sentence) {
  std::string result;

  std::for_each(sentence.begin(), sentence.end(), [&result](const char& c) {
    if (boost::is_any_of(RegexMatcher::REGEX_MARKERS)(c)) {
      result += std::string(" ") + c + std::string(" ");
    } else {
      result += c;
    }
  });
  return result;
}

bool RegexMatcher::match(const std::string& input, const std::string& expr) {
  std::regex expression(expr);
  return std::regex_match(input, expression);
}

bool RegexMatcher::isRegexMarker(const std::string& input) {
  return input.size() == 1 && boost::is_any_of(REGEX_MARKERS)(input[0]);
}

bool RegexMatcher::isEntityRegexMarker(const int entity) {
  return (static_cast<unsigned int>(entity) >
          (0xFFFF - REGEX_MARKERS.size())) &&
         (entity <= 0xFFFF);
}

int RegexMatcher::regexMarkerToEntityId(const std::string& regexMarker) {
  size_t posFound = REGEX_MARKERS.find(regexMarker);
  return 0xFFFF - posFound;
}
}
