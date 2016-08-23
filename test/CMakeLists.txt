cmake_minimum_required(VERSION 3.2)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)

ENABLE_TESTING()

SET(UNIT_TEST_TARGET ${PROJECT_NAME}-test)
SET(INTEGRATION_TEST_TARGET ${PROJECT_NAME}-integration-test)

# JSON thirdparty
INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_SUBDIRECTORY(tests)


SET(RES_FILES
        res/chatbot_model.json
        res/template_replies.json
        res/intent_dictionary_deserialization_test.json
        res/intent_dictionary_deserialization_invalid_test.json
        res/order_beverage_intent.json
        res/queries_model.json
        res/interpreter_model.txt
        res/interpreter_model_w_errors.txt
        res/chatbot_model_without_intent_story.json)

# Copy ressource files
include(Custom)
COPY_RES_FILES(RES_FILES ${RES_FILES} DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/tests)

ADD_CUSTOM_TARGET(run-unit-tests-cpp
    DEPENDS intent-test
    COMMAND ${CMAKE_BINARY_DIR}/test/tests/intent-test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/tests/
)