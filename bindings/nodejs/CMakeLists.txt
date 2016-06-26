cmake_minimum_required(VERSION 3.2)

ADD_CUSTOM_TARGET(nodejs-binding-build ALL
        DEPENDS
            src/main.cpp
            src/SerialiazableChatbot.cpp
            include/Chatbot.hpp
            include/SerializableChatbot.hpp
        DEPENDS intent-static

        COMMAND export BINARY_DIR=${CMAKE_BINARY_DIR} && node-gyp configure
        COMMAND export BINARY_DIR=${CMAKE_BINARY_DIR} && node-gyp build
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/build/Release/open-intent.node
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/open-intent-chatbot/open-intent.node
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
