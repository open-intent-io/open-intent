cmake_minimum_required(VERSION 3.2)


ADD_CUSTOM_TARGET(open-intent-js
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lib/chatbot-api/open-intent.node ${JS_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SerializableChatbot.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/intentjs/SerializableChatbot.hpp
)

file(GLOB_RECURSE JS_SOURCE_FILES LIST_DIRECTORIES false bin/* config/*
        lib/* project-skeletons/* test/* test-integration/* index.js package.json)

ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/open-intent.tgz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lib/chatbot-api/open-intent.node ${JS_SOURCE_FILES}
        COMMAND echo "Packaging nodejs"
        COMMAND rm -f open-intent-*.tgz
        COMMAND npm pack
        COMMAND mv open-intent-*.tgz open-intent.tgz
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lib/chatbot-api/open-intent.node
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/binding.gyp intent-static ${SOURCE_FILES}
        COMMAND npm install node-gyp
        COMMAND node_modules/.bin/node-gyp configure -C src/
        COMMAND export BINARY_DIR=${CMAKE_BINARY_DIR} && node_modules/.bin/node-gyp rebuild -j 4 -C src/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/build/Release/open-intent.node
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/chatbot-api/open-intent.node
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)



FIND_PROGRAM( DOCKER_PATH docker )

IF(DOCKER_PATH)
    ADD_CUSTOM_TARGET(open-intent-docker
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/open-intent.tgz
            COMMAND docker build -t open-intent .
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    ADD_SUBDIRECTORY(test-integration)
ELSE()
    MESSAGE(WARNING "Docker has not been found. Please note that you will not be able to execute the integration tests.")
ENDIF()


ADD_CUSTOM_TARGET(run-unit-tests-js
        DEPENDS open-intent-js
        COMMAND npm install
        COMMAND npm test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

