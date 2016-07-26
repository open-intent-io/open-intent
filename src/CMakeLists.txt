cmake_minimum_required(VERSION 3.2)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/thirdparty/json/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)

find_package(Boost COMPONENTS system filesystem regex log REQUIRED)

SET(SOURCE_FILES
        chatbot/Chatbot.cpp
        chatbot/ChatbotFactory.cpp
        chatbot/SingleSessionChatbot.cpp
        interpreter/EdgeParser.cpp
        interpreter/SentenceToIntentTranslator.cpp
        interpreter/Interpreter.cpp
        interpreter/ReplyTemplateInterpreter.cpp
        intent_service/EntitiesMatcher.cpp
        utils/Deserializer.cpp
        utils/Levenshtein.cpp
        utils/Logger.cpp
        utils/RegexMatcher.cpp
        utils/Tokenizer.cpp
        utils/TrigramHelper.cpp
        intent_service/Term.cpp
        intent_service/TermIndex.cpp
        intent_service/IntentEncoder.cpp
        intent_service/DictionaryModel.cpp
        intent_service/IntentModel.cpp
        intent_service/IntentMatcher.cpp
        intent_service/IntentService.cpp
        intent_story_service/IntentStoryService.cpp
        intent_story_service/IntentStoryModelSerializer.cpp
)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

ADD_LIBRARY(${PROJECT_NAME}-static STATIC ${SOURCE_FILES} ${HEADER_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}-static pthread ${Boost_LIBRARIES})

if(GCOV_ENABLED AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0 -Wall -fprofile-arcs -ftest-coverage")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
    SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")

    SET(GCOV_LIBRARIES_OPTIONAL "gcov")
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}-static ${GCOV_LIBRARIES_OPTIONAL})
ENDIF()
