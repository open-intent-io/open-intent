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
#ifndef INTENT_CHATBOT_HPP
#define INTENT_CHATBOT_HPP

#include <string>
#include "ChatbotModel.hpp"

namespace intent {
/**
 * \brief A chatbot that treats messages from the user, replies and perform user
 * defined actions.
 *
 * The Chatbot class is a finite state automaton built from an OpenIntent
 * ChatbotModel.
 * It is able to reply and perform user defined actions based on user intents.
 */
class Chatbot {
 public:
  /**
   * A map of variables. The keys are variable names and the values are variable
   * contents.
   */
  typedef std::map<std::string, std::string> VariablesMap;

  typedef std::shared_ptr<Chatbot> SharedPtr;

  /**
   *
   * \brief The Context of the automaton. It contains the current state ID and
   * the user defined variables.
   *
   * Context represents the state of the finite state automaton and the
   * variables going through every
   * node of the IntentStory. The variables are passed as parameters in the user
   * defined actions and are used
   * to replace variable placeholders in a template reply.
   */
  struct Context {
    /**
      * The state of the finite state automaton.
      */
    std::string currentStateId;
  };

  /**
   * \brief UserDefinedActionHandler is the interface the user should implement
   * to handle user defined actions.
   */
  class UserDefinedActionHandler {
   public:
    virtual ~UserDefinedActionHandler() {}

    /**
     * \param action                The action to be performed
     * \param intentVariables       The variables extracted from the last
     * intent.
     * \param userDefinedVariables  The set of variables the user can use to
     * publish variables for \
     * template replies.
     *
     * Operator called when a user defined action must be executed.
     */
    virtual void operator()(const std::string& action,
                            const VariablesMap& intentVariables,
                            VariablesMap& templateRepliesVariables) = 0;

    typedef std::shared_ptr<UserDefinedActionHandler> SharedPtr;
  };

 private:
  friend class Interpreter;

 public:
  /**
   * \brief Contructor.
   *
   * Constructs a chatbot from a datamodel.
   *
   * \param chatbotModel  The ChatbotModel to be used by the Chatbot.
   */
  Chatbot(const ChatbotModel& chatbotModel);

  /**
   * \param message   The input message from which to extract an intent.
   * \param context   The context from which you want to match the intent.
   * \param userDefinedActionHandler The implementation of the user defined
   * action handler provided by the user.
   *
   * \brief Handles the user intent going from the state in the context (execute
   * associated user actions).
   *
   * The chatbot will try to find an intent in the message so that it can
   * execute the transition of the
   * finite state automaton to the next state.
   */
  void treatMessage(const std::string& message, Context& context,
                    UserDefinedActionHandler& userDefinedActionHandler,
                    VariablesMap& intentVariables,
                    VariablesMap& userDefinedVariables);

  /**
   * \param actionId             The action id for reply templates retrieval
   * \param intentVariables      The variables parsed from the detected intent
   * \param userDefinedVariables The variables exposed by the user commands
   * \param replies              Output the prepared replies
   *
   * \brief prepares replies and executes user reply handler
   */

  std::vector<std::string> prepareReplies(const std::string& actionId,
                      const Chatbot::VariablesMap& intentVariables,
                      Chatbot::VariablesMap& userDefinedVariables);

  /**
   * \brief Returns the initial state of the model
   */
  std::string getInitialState() const;

  /**
   * \brief Returns the set of terminal states of the model
   */
  std::unordered_set<std::string> getTerminalStates() const;

 protected:
  /** The chatbot action model */
  ChatbotActionModel::SharedPtr m_chatbotActionModel;

  /** An intent story service to match intents */
  IntentStoryService m_intentStoryService;
};
}

#endif
