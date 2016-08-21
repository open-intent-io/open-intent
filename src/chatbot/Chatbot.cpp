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
#include <ctime>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "intent/chatbot/Chatbot.hpp"

#include "intent/utils/Logger.hpp"

namespace intent {

Chatbot::Chatbot(const ChatbotModel& chatbotModel)
    : m_chatbotActionModel(chatbotModel.chatbotActionModel),
      m_intentStoryService(chatbotModel.intentStoryServiceModel) {}

void buildParams(Chatbot::VariablesMap& params,
                 IntentStoryService::Intent& intent) {
  std::for_each(intent.entityMatches.begin(), intent.entityMatches.end(),
                [&params](const IntentStoryService::EntityMatch& match) {
                  params[match.name] =
                      match.term.empty() ? match.text : match.term;
                });
}

template <typename VariablesMap>
std::string replaceVariables(const std::string& input,
                             const boost::regex& regex,
                             const VariablesMap& variables) {
  std::string message = input;
  std::string::const_iterator start, end;
  start = input.begin();
  end = input.end();
  boost::match_results<std::string::const_iterator> what;
  boost::match_flag_type flags = boost::match_default;
  while (regex_search(start, end, what, regex, flags)) {
    const std::string& variablePlaceholder = what[0];
    const std::string& variableName = what[1];

    typename VariablesMap::const_iterator variableIt =
        variables.find(variableName);
    if (variableIt != variables.end()) {
      boost::algorithm::replace_all(message, variablePlaceholder,
                                    variableIt->second);
    }

    // update search position:
    start = what[0].second;
    // update flags:
    flags |= boost::match_prev_avail;
    flags |= boost::match_not_bob;
  }

  return message;
}

std::string replaceTemplateVariables(
    const std::string& templateMessage,
    const Chatbot::VariablesMap& intentVariables,
    Chatbot::VariablesMap& userDefinedVariables) {
  std::string message;

  boost::regex expression;

  expression = "\\$\\{([a-zA-Z0-9_@]+)\\}";
  message = replaceVariables(templateMessage, expression, userDefinedVariables);

  expression = "\\$\\[([a-zA-Z0-9_@]+)\\]";
  message = replaceVariables(message, expression, intentVariables);

  return message;
}

std::vector<std::string> Chatbot::prepareReplies(
    const std::string& actionId, const Chatbot::VariablesMap& intentVariables,
    Chatbot::VariablesMap& userDefinedVariables) {
  INTENT_LOG_INFO() << "Chatbot::prepareReplies starting";

  INTENT_LOG_INFO() << "Chatbot::prepareReplies looking for reply : "
                    << actionId;
  std::vector<std::string> replies;
  ChatbotActionModel::ReplyIdsByActionIdIndex::const_iterator repliesIt =
      m_chatbotActionModel->replyIdsByActionId.find(actionId);

  if (repliesIt != m_chatbotActionModel->replyIdsByActionId.end()) {
    const std::vector<ChatbotActionModel::IndexType>& replyIds =
        repliesIt->second;
    INTENT_LOG_INFO() << "Chatbot::prepareReplies found reply";

    for (const ChatbotActionModel::IndexType& replyId : replyIds) {
      ChatbotActionModel::ReplyContentByReplyIdIndex::const_iterator replyIt =
          m_chatbotActionModel->replyContentByReplyIdIndex.find(replyId);
      if (replyIt != m_chatbotActionModel->replyContentByReplyIdIndex.end()) {
        replies.push_back(replaceTemplateVariables(
            replyIt->second, intentVariables, userDefinedVariables));
        INTENT_LOG_INFO() << "Chatbot::prepareReplies"
                          << replaceTemplateVariables(replyIt->second,
                                                      intentVariables,
                                                      userDefinedVariables);
      }
    }
  }
  return replies;
}

void executeActions(const ChatbotActionModel& chatbotActionModel,
                    const std::string& actionId,
                    Chatbot::UserDefinedActionHandler& userDefinedActionHandler,
                    const Chatbot::VariablesMap& intentVariables,
                    Chatbot::VariablesMap& templateRepliesVariables) {
  INTENT_LOG_INFO() << "Execute user defined action \"" + actionId + "\".";
  // Call the user defined action handler with the actionId.

  userDefinedActionHandler(actionId, intentVariables, templateRepliesVariables);
}

bool Chatbot::treatMessage(const std::string& msg, Context& context,
                           UserDefinedActionHandler& userDefinedActionHandler,
                           Chatbot::VariablesMap& intentVariables,
                           Chatbot::VariablesMap& userDefinedVariables) {
  IntentStoryService::Result result =
      m_intentStoryService.evaluate(context.currentStateId, msg);
  if (result.found) {
    buildParams(intentVariables, result.intent);

    INTENT_LOG_DEBUG() << "Next state is \"" + result.nextStateId + "\".";
    if (m_intentStoryService.getIntentStoryServiceModel()
            .intentStoryModel->isStateIdTerminal(result.nextStateId)) {
      INTENT_LOG_DEBUG() << "Next state (" + result.nextStateId +
                                ") is terminal.";
      context.currentStateId = m_intentStoryService.getIntentStoryServiceModel()
                                   .intentStoryModel->rootStateId;
    } else {
      context.currentStateId = result.nextStateId;
    }

    executeActions(*m_chatbotActionModel, result.actionId,
                   userDefinedActionHandler, intentVariables,
                   userDefinedVariables);
  }

  return result.found;
}

std::string Chatbot::getInitialState() const {
  return m_intentStoryService.getIntentStoryServiceModel()
      .intentStoryModel->rootStateId;
}

std::unordered_set<std::string> Chatbot::getTerminalStates() const {
  return m_intentStoryService.getIntentStoryServiceModel()
      .intentStoryModel->terminalStateIds;
}
}
