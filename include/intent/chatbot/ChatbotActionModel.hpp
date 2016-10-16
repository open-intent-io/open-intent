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
#ifndef INTENT_CHATBOTACTIONMODEL_HPP
#define INTENT_CHATBOTACTIONMODEL_HPP

#include "intent/intent_story_service/IntentStoryService.hpp"

namespace intent {
/**
 * \brief ChatbotActionModel is an action model bound to each state of the
 * intent story finite state automaton
 *
 * This class represents the datamodel that binds actions to the intent story
 * finite state automaton.
 * This datamodel contains the cutomized replies and user defined actions as
 * well as the binding between those
 * actions and the corresponding states in the finite state automaton.
 * The model also contains the fallback replies that will be used by the chatbot
 * if it does not understand
 * the user intent.
 */
class ChatbotActionModel {
 public:
  typedef IntentStoryModel::IndexType IndexType;
  typedef std::map<IndexType, std::string> ReplyContentByReplyIdIndex;
  typedef std::shared_ptr<ChatbotActionModel> SharedPtr;

  struct StateAndActionId
  {
      IndexType actionId;
      IndexType state;

      bool operator<(const StateAndActionId& that) const
      {
        if(state != that.state)
          return state < that.state;

        return actionId < that.actionId;
      }
  };

  typedef std::map<StateAndActionId, std::vector<IndexType> > ReplyIdsByStateAndActionIdIndex;

  /**
   * Index of list of reply IDs by action id. The action can be a reply or a
   * user defined action
   */
  ReplyIdsByStateAndActionIdIndex replyIdsByStateAndActionId;

  /**
   * Index of replies by reply id.
   */
  ReplyContentByReplyIdIndex replyContentByReplyIdIndex;
};
}

#endif  // INTENT_CHATBOTMODEL_HPP
