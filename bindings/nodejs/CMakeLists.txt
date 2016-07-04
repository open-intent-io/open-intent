cmake_minimum_required(VERSION 3.2)

ADD_CUSTOM_TARGET(nodejs-binding-build ALL
        DEPENDS
            src/main.cpp
            src/SerialiazableChatbot.cpp
            include/SerializableChatbot.hpp
        DEPENDS intent-static

        COMMAND export BINARY_DIR=${CMAKE_BINARY_DIR} && node-gyp configure
        COMMAND export BINARY_DIR=${CMAKE_BINARY_DIR} && node-gyp build
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/build/Release/open-intent.node
            ${CMAKE_CURRENT_SOURCE_DIR}/modules/open-intent/chatbot-api/open-intent.node
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_CUSTOM_COMMAND(TARGET nodejs-binding-build POST_BUILD
        COMMAND npm install
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/modules/open-intent
)

ADD_CUSTOM_TARGET(run-nodejs-tests)

ADD_CUSTOM_COMMAND(TARGET run-nodejs-tests POST_BUILD
        COMMAND npm test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/modules/open-intent
)

ADD_CUSTOM_COMMAND(TARGET run-nodejs-tests POST_BUILD
        COMMAND npm install && npm test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/rest-chatbot
)