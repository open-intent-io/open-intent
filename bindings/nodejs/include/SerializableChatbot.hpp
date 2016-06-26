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
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef INTENTJS_REST_CHATBOT_HPP
#define INTENTJS_REST_CHATBOT_HPP

#include <node.h>
#include <node_object_wrap.h>

#include "intent/chatbot/Chatbot.hpp"

namespace intentjs
{
    typedef intent::Chatbot iChatbot;
    using namespace v8;

    class SerializableReplyHandler : public intent::Chatbot::ReplyActionHandler
    {
    public:
        SerializableReplyHandler(Isolate *isolate, Local <Function> cb)
        {
            m_isolate = isolate;
            m_callback = cb;
        }

        void operator()(const std::string &message)
        {
            const unsigned argc = 1;
            Local <Value> argv[argc] = {v8::String::NewFromUtf8(m_isolate, message.c_str())};
            m_callback->Call(Null(m_isolate), argc, argv);
        }

    private:
        Local <Function> m_callback;
        Isolate *m_isolate;
    };

    class SerializableUserDefinedActionsHandler : public intent::Chatbot::UserDefinedActionHandler
    {
    public:
        SerializableUserDefinedActionsHandler(Isolate *isolate, Local <Function> cb, Local<Object>& v8context, intent::Chatbot::Context& context)
            :
              m_context(context),
              m_v8context(v8context)
        {
            m_isolate = isolate;
            m_callback = cb;
        }

        void operator()(const std::string &action,
                        const intent::Chatbot::VariablesMap &intentVariables,
                        intent::Chatbot::VariablesMap &userDefinedVariables)
        {
            //set the new state in context for it to be saved correctly
            m_v8context->Set(String::NewFromUtf8(m_isolate, "_state"),
                             String::NewFromUtf8(m_isolate, m_context.currentStateId.c_str()));

            const unsigned argc = 3;
            Local <Object> intentVariablesObject = Object::New(m_isolate);
            Local <Object> userDefinedVariablesObject = Object::New(m_isolate);

            for (intent::Chatbot::VariablesMap::const_reference &v: intentVariables)
            {
                intentVariablesObject->Set(String::NewFromUtf8(m_isolate, v.first.c_str()),
                                           String::NewFromUtf8(m_isolate, v.second.c_str()));
            }
            for (intent::Chatbot::VariablesMap::const_reference &v: userDefinedVariables)
            {
                userDefinedVariablesObject->Set(String::NewFromUtf8(m_isolate, v.first.c_str()),
                                                String::NewFromUtf8(m_isolate, v.second.c_str()));
            }

            Local <Value> argv[argc] = {
                    v8::String::NewFromUtf8(m_isolate, action.c_str()),
                    intentVariablesObject,
                    userDefinedVariablesObject
            };

            {
                HandleScope scope(m_isolate);
                v8::TryCatch try_catch;

                Local <Function> f = Local<Function>::New(m_isolate, m_callback);
                f->Call(Null(m_isolate), argc, argv);

                if(try_catch.HasCaught())
                {
                    Local<Value> exception = try_catch.Exception();
                    //v8::Handle<v8::Message> message = try_catch.Message();
                    //Local<Value> stackTrace = try_catch.StackTrace();

                    if (!exception.IsEmpty())
                    {
                        String::Utf8Value exception_str(exception);
                        std::string exceptionStdString(*exception_str);
                        std::cerr << exceptionStdString << std::endl;
                    }
                    else
                    {
                        std::cerr << "Exception thrown" << std::endl;
                    }
                }
            }
        }

    private:
        Local <Function> m_callback;
        Isolate *m_isolate;
        intent::Chatbot::Context& m_context;
        Local<Object>& m_v8context;
    };

    class SerializableChatbot : public node::ObjectWrap
    {
    public:
        static void Init(v8::Isolate *isolate);

        static void InstantiateFromJsonModel(const v8::FunctionCallbackInfo <v8::Value> &args);

        static void InstantiateFromOIML(const v8::FunctionCallbackInfo <v8::Value> &args);

    private:
        explicit SerializableChatbot(iChatbot::SharedPtr chatbot);

        ~SerializableChatbot()
        { }

        static void TreatMessage(const v8::FunctionCallbackInfo <v8::Value> &args);
        static void GetInitialState(const v8::FunctionCallbackInfo <v8::Value> &args);
        static void GetTerminalStates(const v8::FunctionCallbackInfo <v8::Value> &args);
        static void PrepareReplies(const v8::FunctionCallbackInfo <v8::Value> &args);

        static v8::Persistent <v8::Function> constructorFromJsonModel;
        static v8::Persistent <v8::Function> constructorFromOIML;

    public:
        iChatbot::SharedPtr m_chatbot;
    };


}


#endif
