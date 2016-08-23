cmake_minimum_required(VERSION 3.2)


ADD_CUSTOM_TARGET(open-intent-js
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lib/chatbot-api/open-intent.node
)

SET(SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SerializableChatbot.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/intentjs/SerializableChatbot.hpp
)

ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lib/chatbot-api/open-intent.node
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/binding.gyp intent-static ${SOURCE_FILES}
        COMMAND node-gyp configure
        COMMAND export BINARY_DIR=${CMAKE_BINARY_DIR} && node-gyp rebuild -j 4
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/build/Release/open-intent.node
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/chatbot-api/open-intent.node
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)



FIND_PROGRAM( DOCKER_PATH docker )

IF(DOCKER_PATH)
    ADD_CUSTOM_TARGET(open-intent-docker
            DEPENDS open-intent-js
            COMMAND docker build -t open-intent .
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    ADD_SUBDIRECTORY(test-integration)
ELSE()
    MESSAGE(WARNING "Docker has not been found. Please note that you will not be able to execute the integration tests.")
ENDIF()


ADD_CUSTOM_TARGET(run-unit-tests-js
    DEPENDS open-intent-js
    COMMAND npm test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

