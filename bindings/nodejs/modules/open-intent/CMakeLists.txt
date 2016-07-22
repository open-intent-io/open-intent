cmake_minimum_required(VERSION 3.2)

ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(test-integration)

ADD_CUSTOM_TARGET(nodejs-module-docker-build
        DEPENDS nodejs-binding-build
        COMMAND docker build -t open-intent ${CMAKE_CURRENT_SOURCE_DIR}
)
