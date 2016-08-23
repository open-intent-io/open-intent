cmake_minimum_required(VERSION 3.2)

ADD_CUSTOM_TARGET(run-integration-tests-js
        DEPENDS run-integration-tests-js-chatbot run-integration-tests-js-tutorial
)

ADD_CUSTOM_TARGET(run-integration-tests-js-chatbot
        DEPENDS open-intent-docker
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/launch.sh chatbot
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_CUSTOM_TARGET(run-integration-tests-js-tutorial
        DEPENDS open-intent-docker
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/launch.sh tutorial
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

