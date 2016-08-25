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
#include "intent/interpreter/ScenarioIndexer.hpp"

#include <boost/algorithm/string.hpp>

#include "intent/interpreter/LineTagger.hpp"
#include "intent/utils/SingleCharacterDelimiterTokenizer.hpp"

namespace intent {

void indexScenario(const Scenario& scenario,
                   InquiryToReplies& inquiryToReplies) {
  int index = 0;
  int counter = 0;
  bool isInSaying = 0;

  InquiryToReply inquiryToReply;
  std::for_each(
      scenario.begin(), scenario.end(),
      [&inquiryToReplies, &inquiryToReply, &index, &counter,
       &isInSaying](const ScriptLine& line) {
        LineRange& inquiry = inquiryToReply.first;
        LineRange& reply = inquiryToReply.second;

        if (isLine<SAYING>(line)) {
          if (counter % 2 == 0) {
            inquiry.lower = index;
            inquiry.upper = index;
            reply.lower = -1;
          }
          if (counter % 2 == 1) {
            reply.lower = index;
            reply.upper = index;
          }
          isInSaying = true;
          ++counter;
        } else if (!isMarkedLine(line) && isInSaying) {
          if (reply.lower == -1) {
            inquiry.upper++;
          } else {
            reply.upper++;
          }
        } else if (isMarkedLine(line) && isInSaying && reply.lower != -1) {
          inquiryToReplies.insert(inquiryToReply);
          isInSaying = false;
        } else {
          isInSaying = false;
        }
        ++index;
      });
}

std::vector<ScriptLine> tokenizeInLines(const std::string& input) {
  std::vector<std::string> lines;
  std::vector<char> delimiters = {'\n'};
  SingleCharacterDelimiterTokenizer::tokenize(input, delimiters, lines);
  std::vector<ScriptLine> scriptLines;
  for (unsigned int i = 0; i < lines.size(); ++i) {
    scriptLines.push_back({lines[i], i});
  }
  return scriptLines;
}

void extractScenarios(const std::string& script, Scenarios& scenarios) {
  int braceCounter = 0;
  Scenario scenario;
  std::vector<ScriptLine> lines = tokenizeInLines(script);

  std::for_each(lines.begin(), lines.end(),
                [&scenarios, &scenario, &braceCounter](ScriptLine& line) {
                  boost::trim(line.content);
                  if (isLine<CLOSE_SCENARIO>(line)) {
                    --braceCounter;
                  }
                  if (braceCounter == 1) {
                    scenario.push_back(line);
                  }
                  if (isLine<START_SCENARIO>(line)) {
                    ++braceCounter;
                  }
                  if (braceCounter == 0) {
                    if (!scenario.empty()) scenarios.push_back(scenario);
                    scenario.clear();
                  }

                });
}
}
