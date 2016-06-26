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
#ifndef INTENTJS_CHATBOT_HPP
#define INTENTJS_CHATBOT_HPP

#include <node.h>
#include <node_object_wrap.h>

#include "intent/chatbot/Chatbot.hpp"
#include "intent/chatbot/SingleSessionChatbot.hpp"

namespace intentjs
{
    using namespace v8;

    class ReplyHandler : public intent::Chatbot::ReplyActionHandler
    {
        public:
            ReplyHandler(Isolate *isolate, Local<Function> cb) 
            {
                m_isolate = isolate;
                m_callback.Reset(m_isolate, cb);
            }

            void operator()(const std::string &message)
            {
                const unsigned argc = 1;
                Local<Value> argv[argc] = { v8::String::NewFromUtf8(m_isolate, message.c_str()) };
                Local<Function> f = Local<Function>::New(m_isolate, m_callback);
                f->Call(Null(m_isolate), argc, argv);
            }

        private:
            Persistent<Function> m_callback;
            Isolate *m_isolate;
    };

    class UserDefinedActionsHandler : public intent::Chatbot::UserDefinedActionHandler
    {
        public:
            UserDefinedActionsHandler(Isolate *isolate, Local<Function> cb) 
            {
                m_isolate = isolate;
                m_callback.Reset(m_isolate, cb);
            }

            void operator()(const std::string &action, 
                    const intent::Chatbot::VariablesMap &intentVariables, 
                    intent::Chatbot::VariablesMap &userDefinedVariables)
            {
                const unsigned argc = 3;
                Local<Object> intentVariablesObject = Object::New(m_isolate);
                Local<Object> userDefinedVariablesObject = Object::New(m_isolate);

                for(intent::Chatbot::VariablesMap::const_reference &v: intentVariables)
                {
                    intentVariablesObject->Set(String::NewFromUtf8(m_isolate, v.first.c_str()), String::NewFromUtf8(m_isolate, v.second.c_str()));
                }
                for(intent::Chatbot::VariablesMap::const_reference &v: userDefinedVariables)
                {
                    userDefinedVariablesObject->Set(String::NewFromUtf8(m_isolate, v.first.c_str()), String::NewFromUtf8(m_isolate, v.second.c_str()));
                }

                Local<Value> argv[argc] = { 
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

                Local<Array> property_names = userDefinedVariablesObject->GetOwnPropertyNames();
                userDefinedVariables.clear();

                for (unsigned int i = 0; i < property_names->Length(); ++i) {
                    Local<Value> key = property_names->Get(i);
                    Local<Value> value = userDefinedVariablesObject->Get(key);

                    if (key->IsString() && value->IsString()) {
                        String::Utf8Value utf8_key(key);
                        String::Utf8Value utf8_value(value);
                        std::string key = std::string(*utf8_key);
                        std::string value = std::string(*utf8_value);
                        userDefinedVariables[key] = value;
                    }
                    else 
                    {
                        m_isolate->ThrowException(v8::Exception::TypeError(
                                    String::NewFromUtf8(m_isolate, "You can only publish string variables.")));
                        return;
                    }
                }

            }
        private:
            Persistent<Function> m_callback;
            Isolate *m_isolate;
    };

}


#endif
