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
#include <node.h>
#include <v8.h>
#include <iostream>
#include "SerializableChatbot.hpp"
#include "intent/chatbot/ChatbotFactory.hpp"
#include "intent/utils/Logger.hpp"

using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;

namespace intentjs
{

    namespace
    {

        void parseV8String(Isolate *isolate, const Local <String> &currentStateStr, std::string &currentState)
        {
            v8::String::Utf8Value utf8Value(currentStateStr);
            std::string content(*utf8Value);
            currentState = content.c_str();
        }

        void parseContext(Isolate *isolate, const Local <Object> &sessionContext, intent::Chatbot::Context &context)
        {
            Local <Value> currentStateKey = v8::String::NewFromUtf8(isolate, "_state");
            if (sessionContext->Has(currentStateKey))
            {
                Local <Value> currentStateValue = sessionContext->Get(currentStateKey);

                if (currentStateValue->IsString())
                {
                    parseV8String(isolate, currentStateValue->ToString(), context.currentStateId);
                }
                else
                {
                    std::string error = "Current state must be a string.";
                    LOG_ERROR() << error << "\n";

                    isolate->ThrowException(v8::Exception::TypeError(
                            String::NewFromUtf8(isolate, error.c_str())));
                }
            }
        }
    }

    SerializableChatbot::SerializableChatbot(iChatbot::SharedPtr chatbot) :
            m_chatbot(chatbot)
    { }

    Persistent <Function> SerializableChatbot::constructorFromJsonModel;
    Persistent <Function> SerializableChatbot::constructorFromOIML;

    void SerializableChatbot::Init(Isolate *isolate)
    {

        Local <FunctionTemplate> fromJsonModelTemplate = FunctionTemplate::New(isolate, InstantiateFromJsonModel);
        fromJsonModelTemplate->SetClassName(String::NewFromUtf8(isolate, "SerializableChatbot"));
        fromJsonModelTemplate->InstanceTemplate()->SetInternalFieldCount(4);

        // Prototype
        NODE_SET_PROTOTYPE_METHOD(fromJsonModelTemplate, "treatMessage", TreatMessage);
        NODE_SET_PROTOTYPE_METHOD(fromJsonModelTemplate, "getInitialState", GetInitialState);
        NODE_SET_PROTOTYPE_METHOD(fromJsonModelTemplate, "getTerminalStates", GetTerminalStates);

        // Prototype
        NODE_SET_PROTOTYPE_METHOD(fromJsonModelTemplate, "prepareReplies", PrepareReplies);

        constructorFromJsonModel.Reset(isolate, fromJsonModelTemplate->GetFunction());


        Local <FunctionTemplate> fromOIMLTemplate = FunctionTemplate::New(isolate, InstantiateFromOIML);
        fromOIMLTemplate->SetClassName(String::NewFromUtf8(isolate, "SerializableChatbot"));
        fromOIMLTemplate->InstanceTemplate()->SetInternalFieldCount(4);

        // Prototype
        NODE_SET_PROTOTYPE_METHOD(fromOIMLTemplate, "treatMessage", TreatMessage);
        NODE_SET_PROTOTYPE_METHOD(fromOIMLTemplate, "getInitialState", GetInitialState);
        NODE_SET_PROTOTYPE_METHOD(fromOIMLTemplate, "getTerminalStates", GetTerminalStates);

        // Prototype
        NODE_SET_PROTOTYPE_METHOD(fromOIMLTemplate, "prepareReplies", PrepareReplies);

        constructorFromOIML.Reset(isolate, fromOIMLTemplate->GetFunction());
    }

    /**
     * @brief SerializableChatbot::TreatMessage
     * @param args : Context, Message, replyCallback, actionCallback,
     */
    void SerializableChatbot::TreatMessage(const FunctionCallbackInfo <Value> &args)
    {
        LOG_TRACE() << "SerializableChatbot::TreatMessage\n";

        Isolate *isolate = args.GetIsolate();
        SerializableChatbot *obj = ObjectWrap::Unwrap<SerializableChatbot>(args.Holder());

        if (!args[0]->IsObject())
        {
            std::string error = "Context must be an object.";
            LOG_ERROR() << error << "\n";
            isolate->ThrowException(v8::Exception::TypeError(
                    String::NewFromUtf8(isolate, error.c_str())));
            return;
        }
        Local <Object> sessionContext = Local<Object>::Cast(args[0]);

        if (!args[1]->IsString())
        {
            std::string error = "The message must be a string.";
            LOG_ERROR() << error << "\n";
            isolate->ThrowException(v8::Exception::TypeError(
                    String::NewFromUtf8(isolate, error.c_str())));
            return;
        }
        v8::String::Utf8Value messageValue(args[1]->ToString());

        if (!args[2]->IsFunction())
        {
            std::string error = "The user defined actions handler must be a function.";
            LOG_ERROR() << error << "\n";
            isolate->ThrowException(v8::Exception::TypeError(
                    String::NewFromUtf8(isolate, error.c_str())));
            return;
        }
        Local <Function> userDefinedActionsCallback = Local<Function>::Cast(args[2]);

        intent::Chatbot::Context context;
        parseContext(isolate, sessionContext, context);
        SerializableUserDefinedActionsHandler userDefinedActionHandler(isolate, userDefinedActionsCallback, sessionContext, context);

        std::string message = std::string(*messageValue);

        intent::Chatbot::VariablesMap userDefinedVariables;
        intent::Chatbot::VariablesMap intentVariables;

        LOG_TRACE() << "Call to C++ method treatMessage from chatbot";
        bool intentFound = obj->m_chatbot->treatMessage(message, context, userDefinedActionHandler, intentVariables, userDefinedVariables);

        args.GetReturnValue().Set(intentFound);
    }


    void extractMapFromObject(Local<Object>& object, intent::Chatbot::VariablesMap& map)
    {
        LOG_TRACE() << "Extracting variables from v8 map : \n";
        Local<Array> property_names = object->GetOwnPropertyNames();

        for (unsigned int i = 0; i < property_names->Length(); ++i) {
            Local<Value> v8key = property_names->Get(i);
            Local<Value> v8value = object->Get(v8key);
            String::Utf8Value utf8_key(v8key);
            std::string key(*utf8_key);
            if (v8value->IsString()) {
                String::Utf8Value utf8_value(v8value);
                std::string value(*utf8_value);
                map[key] = value;
            } else {
                // Throw an error or something
            }
        }
    }

    /**
     * @brief SerializableChatbot::PrepareReplies
     * @param args : Context, IntentVariables, UserVariables, replyCallback
     */
    void SerializableChatbot::PrepareReplies(const FunctionCallbackInfo <Value> &args)
    {
        Isolate *isolate = args.GetIsolate();
        SerializableChatbot *obj = ObjectWrap::Unwrap<SerializableChatbot>(args.Holder());
        LOG_TRACE() << "SerializableChatbot::PrepareReplies" << "\n";

        if (!args[0]->IsString())
        {
            isolate->ThrowException(v8::Exception::TypeError(
                    String::NewFromUtf8(isolate, "actionId must be a String")));
            return;
        }
        std::string actionId;
        parseV8String(isolate, args[0]->ToString(), actionId);

        if (!args[1]->IsObject())
        {
            isolate->ThrowException(v8::Exception::TypeError(
                    String::NewFromUtf8(isolate, "IntentVariables must be a map.")));
            return;
        }
        Local<Object> v8intentVariables = Local<Object>::Cast(args[1]);
        intent::Chatbot::VariablesMap intentVariables;
        extractMapFromObject(v8intentVariables, intentVariables);

        if (!args[2]->IsObject())
        {
            isolate->ThrowException(v8::Exception::TypeError(
                    String::NewFromUtf8(isolate, "UserVariables must be a map.")));
            return;
        }
        Local<Object> v8userVariables = Local<Object>::Cast(args[2]);
        intent::Chatbot::VariablesMap userVariables;
        extractMapFromObject(v8userVariables, userVariables);

        std::vector<std::string> replies = obj->m_chatbot->prepareReplies(actionId, intentVariables, userVariables);

        LOG_TRACE() << "Chatbot::prepareReplies " << "\n";
        Local<Array> repliesArray = Array::New(isolate);
        unsigned int i=0;

        for(const std::string &reply: replies)
        {
            Local <Value> v8reply = v8::String::NewFromUtf8(isolate, reply.c_str());
            repliesArray->Set(i++, v8reply);
        }

        args.GetReturnValue().Set(repliesArray);
    }

    void SerializableChatbot::GetInitialState(const FunctionCallbackInfo <Value> &args)
    {
        Isolate *isolate = args.GetIsolate();
        SerializableChatbot *obj = ObjectWrap::Unwrap<SerializableChatbot>(args.Holder());

        Local <Value> initialState = v8::String::NewFromUtf8(isolate, obj->m_chatbot->getInitialState().c_str());

        args.GetReturnValue().Set(initialState);
    }

    void SerializableChatbot::GetTerminalStates(const FunctionCallbackInfo <Value> &args)
    {
        Isolate *isolate = args.GetIsolate();
        SerializableChatbot *obj = ObjectWrap::Unwrap<SerializableChatbot>(args.Holder());

        Local<Array> terminalStates = Array::New(isolate);
        unsigned int i=0;

        for(const std::string &state: obj->m_chatbot->getTerminalStates())
        {
            Local <Value> v8state = v8::String::NewFromUtf8(isolate, state.c_str());
            terminalStates->Set(i++, v8state);
        }

        args.GetReturnValue().Set(terminalStates);
    }

    void SerializableChatbot::InstantiateFromJsonModel(const v8::FunctionCallbackInfo <v8::Value> &args)
    {
        Isolate *isolate = args.GetIsolate();

        Local <Value> jsonModelFilepathValue(args[0]->ToString());

        if (args.IsConstructCall())
        {
            v8::String::Utf8Value v0(jsonModelFilepathValue);
            std::string jsonModel = std::string(*v0);

            std::stringstream jsonModelStream;
            jsonModelStream << jsonModel;

            iChatbot::SharedPtr chatbot =
                    intent::ChatbotFactory::createChatbotFromJsonModel(jsonModelStream);

            if (!chatbot.get())
            {
                std::string error = "Error while creating chatbot.";
                LOG_ERROR() << error << "\n";
                isolate->ThrowException(v8::Exception::TypeError(
                        String::NewFromUtf8(isolate, error.c_str())));
            }

            SerializableChatbot *obj = new SerializableChatbot(chatbot);

            obj->Wrap(args.This());
            args.GetReturnValue().Set(args.This());
        }
        else
        {
            int argc = 1;
            Local <Value> argv[argc] = {jsonModelFilepathValue};

            Local <Context> context = isolate->GetCurrentContext();
            Local <Function> cons = Local<Function>::New(isolate, constructorFromJsonModel);

            MaybeLocal <Object> instance =
                    cons->NewInstance(context, argc, argv);

            if (!instance.IsEmpty())
                args.GetReturnValue().Set(instance.ToLocalChecked());
        }
    }

    void wrapInterpreterFeedback(Isolate* isolate, Local<Object>& v8feedback, intent::InterpreterFeedback& feedback)
    {
        LOG_TRACE() << "Collecting interpreter messages" << "\n";
        Local<Array> interpretMessages = Array::New(isolate, (int)feedback.size());
        for (unsigned int i = 0; i<feedback.size(); ++i)
        {
            Local<Object> toInsert = Object::New(isolate);
            toInsert->Set(String::NewFromUtf8(isolate, "line"), Number::New(isolate, feedback[i].line.position));
            toInsert->Set(String::NewFromUtf8(isolate, "message"), String::NewFromUtf8(isolate, feedback[i].message.c_str()));
            toInsert->Set(String::NewFromUtf8(isolate, "level"), Number::New(isolate, feedback[i].level));
            interpretMessages->Set(i, toInsert);
        }
        v8feedback->Set(String::NewFromUtf8(isolate, "messages"), interpretMessages);
    }

    void SerializableChatbot::InstantiateFromOIML(const v8::FunctionCallbackInfo <v8::Value> &args)
    {
        Isolate *isolate = args.GetIsolate();

        if (!args[0]->IsString())
        {
            isolate->ThrowException(v8::Exception::TypeError(
                    String::NewFromUtf8(isolate, "dictionary must be a string")));
            return;
        }
        Local <Value> dictionaryValue(args[0]->ToString());

        if (!args[1]->IsString())
        {
            isolate->ThrowException(v8::Exception::TypeError(
                    String::NewFromUtf8(isolate, "script must be a string")));
            return;
        }
        Local <Value> oimlValue(args[1]->ToString());

        if (args.IsConstructCall())
        {
            v8::String::Utf8Value v0(dictionaryValue);
            v8::String::Utf8Value v1(oimlValue);

            std::string dictionaryModel = std::string(*v0);
            std::string interpreterModel = std::string(*v1);

            std::stringstream dictionaryStream, interpreterModelStream;
            dictionaryStream << dictionaryModel;
            interpreterModelStream << interpreterModel;

            intent::InterpreterFeedback feedback;
            iChatbot::SharedPtr chatbot =
                    intent::ChatbotFactory::createChatbotFromOIML(
                            dictionaryStream, interpreterModelStream, feedback);

            if(!feedback.empty())
            {
                Local <Object> interpreterFeedback = Object::New(isolate);
                wrapInterpreterFeedback(isolate, interpreterFeedback, feedback);
                std::string error = "Error while interpreting the model.";
                LOG_ERROR() << error << "\n";
                isolate->ThrowException(interpreterFeedback);
            }
            else if (!chatbot.get())
            {
                std::string error = "Error while creating chatbot.";
                LOG_ERROR() << error << "\n";
                isolate->ThrowException(v8::Exception::TypeError(
                        String::NewFromUtf8(isolate, error.c_str())));
            }

            SerializableChatbot *obj = new SerializableChatbot(chatbot);
            obj->Wrap(args.This());
            args.GetReturnValue().Set(args.This());
        }
        else
        {
            int argc = 3;
            Local <Value> argv[argc] = { dictionaryValue, oimlValue, args[2] };

            Local <Context> context = isolate->GetCurrentContext();
            Local <Function> cons = Local<Function>::New(isolate, constructorFromOIML);

            MaybeLocal <Object> instance =
                    cons->NewInstance(context, argc, argv);

            if (!instance.IsEmpty())
                args.GetReturnValue().Set(instance.ToLocalChecked());
        }
    }

}
