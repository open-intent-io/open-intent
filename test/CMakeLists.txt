cmake_minimum_required(VERSION 3.2)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)

ENABLE_TESTING()

SET(UNIT_TEST_TARGET ${PROJECT_NAME}-test)
SET(INTEGRATION_TEST_TARGET ${PROJECT_NAME}-integration-test)

# JSON thirdparty
INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/thirdparty/json/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/thirdparty/spdlog/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

ADD_CUSTOM_TARGET(run-unit-tests-cpp
    DEPENDS intent-test
    COMMAND ${CMAKE_BINARY_DIR}/test/intent-test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
)

SET(SOURCE_FILES
        launcher/main.cpp
        launcher/ResourceManager.cpp
        launcher/TestContext.cpp

        same_successive_intents/SameSuccessiveIntentsTest.cpp

        ChatbotFactoryTest.cpp
        ChatbotTest.cpp
        EntitiesMatcherTest.cpp
        DeserializerTest.cpp
        GraphTest.cpp
        IntentServiceTest.cpp
        IntentStoryServiceTest.cpp
        MultiSessionChatbotTest.cpp
        SingleCharacterDelimiterTokenizerTest.cpp
        TermIndexTest.cpp
        TokenizerTest.cpp
        TrigramUtilsTest.cpp
        SingleSessionChatbotTest.cpp
        IntentMatcherTest.cpp
        InterpreterTest.cpp
        IntentStoryModelSerializer.cpp
)

SET(HEADER_FILES)

INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

if(GCOV_ENABLED)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -fprofile-arcs -ftest-coverage")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
    SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        include(CodeCoverage)
        setup_target_for_coverage(${UNIT_TEST_TARGET}_coverage ${UNIT_TEST_TARGET} coverage)
    endif()
endif()

add_definitions(-DRESSOURCES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")

ADD_EXECUTABLE(${UNIT_TEST_TARGET} ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(${UNIT_TEST_TARGET} ${PROJECT_NAME}-static
        gtest
        gmock
        gcov
)

