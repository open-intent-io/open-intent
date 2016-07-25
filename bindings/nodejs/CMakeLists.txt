cmake_minimum_required(VERSION 3.2)

ADD_SUBDIRECTORY(modules)

ADD_CUSTOM_TARGET(nodejs-binding-build ALL
        DEPENDS
            src/main.cpp
            src/SerialiazableChatbot.cpp
            include/SerializableChatbot.hpp
        DEPENDS intent-static

        COMMAND export BINARY_DIR=${CMAKE_BINARY_DIR} && node-gyp configure
        COMMAND export BINARY_DIR=${CMAKE_BINARY_DIR} && node-gyp build
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/build/Release/open-intent.node
        modules/open-intent/lib/chatbot-api/open-intent.node
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_CUSTOM_COMMAND(TARGET nodejs-binding-build POST_BUILD
        COMMAND npm install
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/modules/open-intent
)

FIND_PROGRAM( DOCKER_PATH docker )

IF(DOCKER_PATH)
    ADD_CUSTOM_COMMAND(TARGET nodejs-binding-build POST_BUILD
            COMMAND docker build -t open-intent .
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/modules/open-intent
    )
ELSE()
    MESSAGE(WARNING "Docker has not been found. Please note that you will not be able to execute the integration tests.")
ENDIF()
