cmake_minimum_required(VERSION 3.2)

ADD_CUSTOM_TARGET(run-nodejs-tests)

ADD_CUSTOM_COMMAND(TARGET run-nodejs-tests POST_BUILD
        DEPENDS nodejs-binding-build
        COMMAND npm test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
)
