{
    "targets": [
    {
        "target_name": "open-intent",
            "sources": [
                "src/bindings/nodejs/main.cpp",
                "src/bindings/nodejs/SerialiazableChatbot.cpp",

                "src/chatbot/Chatbot.cpp",
                "src/chatbot/ChatbotFactory.cpp",
                "src/chatbot/SingleSessionChatbot.cpp",
                "src/interpreter/EdgeParser.cpp",
                "src/interpreter/SentenceToIntentTranslator.cpp",
                "src/interpreter/Interpreter.cpp",
                "src/interpreter/ReplyTemplateInterpreter.cpp",
                "src/intent_service/EntitiesMatcher.cpp",
                "src/utils/Deserializer.cpp",
                "src/utils/Levenshtein.cpp",
                "src/utils/Logger.cpp",
                "src/utils/RegexMatcher.cpp",
                "src/utils/SingleCharacterDelimiterTokenizer.cpp",
                "src/utils/Tokenizer.cpp",
                "src/utils/TrigramHelper.cpp",
                "src/intent_service/Term.cpp",
                "src/intent_service/TermIndex.cpp",
                "src/intent_service/IntentEncoder.cpp",
                "src/intent_service/DictionaryModel.cpp",
                "src/intent_service/IntentModel.cpp",
                "src/intent_service/IntentMatcher.cpp",
                "src/intent_service/IntentService.cpp",
                "src/intent_service/SentenceTokenizer.cpp",
                "src/intent_story_service/IntentStoryModelSerializer.cpp",
                "src/intent_story_service/IntentStoryService.cpp"
             ],
            'include_dirs': [
                'include', 'thirdparty/json/src'
            ],
            "cflags": [
                "-std=c++11",
                "-frtti",
                "-fexceptions"
            ],
            'cflags_cc': [
                "-std=c++11",
                "-frtti",
                "-fexceptions"
            ],
            'libraries': [
                '-lboost_system', '-lboost_filesystem', '-lboost_regex', '-lboost_log'
            ]
    }
    ]
}
