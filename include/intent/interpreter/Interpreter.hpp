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
#ifndef INTENT_INTERPRETER_HPP
#define INTENT_INTERPRETER_HPP

#include "intent/chatbot/ChatbotModel.hpp"

#include <string>
#include <vector>

namespace intent {
enum message_code {
  ROOT_STATE_MSG,
  TERMINAL_STATE_MSG,
  NO_ENTITY,
  NO_ACTION,
  ANONYMOUS_STATE_CREATION,
  DUPLICATE_ACTION_ID,
  ANONYMOUS_ACTION_CREATION
};

namespace {
const char* messagesText[] = {
    "first line of first scenario should be root state",
    "final line of first scenario should be terminal state",
    "no entity was found for the line",
    "no action was found for the edge",
    "creating anonymous state ",
    "The actionID was specified twice",
    "no action was found for the interaction, anonymous action created"};
}

struct ScriptLine {
  ScriptLine(std::string content, unsigned int position)
      : content(content), position(position) {}

  ScriptLine(std::string content) : content(content), position(0) {}

  std::string content;
  unsigned int position;
};

enum LogLevel { WARNING, ERROR, INFO };

struct InterpreterMessage {
  InterpreterMessage(message_code msg_code, ScriptLine scriptLine,
                     LogLevel level = INFO)
      : level(level), message(messagesText[msg_code]), line(scriptLine) {}

  LogLevel level;
  std::string message;
  ScriptLine line;
};

struct LineRange {
  LineRange(int _lower, int _upper) : lower(_lower), upper(_upper) {}

  LineRange() : lower(-1), upper(-1) {}

  friend bool operator<(const LineRange& l, const LineRange& r) {
    return l.lower < r.lower;
  }

  int lower;
  int upper;
};

typedef std::vector<InterpreterMessage> InterpreterFeedback;
typedef std::vector<ScriptLine> Scenario;
typedef std::vector<Scenario> Scenarios;
typedef std::pair<LineRange, LineRange> InquiryToReply;
typedef std::map<LineRange, LineRange> InquiryToReplies;

const std::string REGEX_MARKERS = "[]|*";

/**
 * \brief Interpreter the OpenIntent chatbot language.
 */
class Interpreter {
 public:
  /**
   * \brief build a ChatbotModel from a script.
   * \param intput The input cript.
   * \return The ChatbotModel.
   */
  static ChatbotModel build(const std::string& script,
                            DictionaryModel::SharedPtr dictionaryModel,
                            InterpreterFeedback& m_interpreterFeedback);
};
}

#endif
