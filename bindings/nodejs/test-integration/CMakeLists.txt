cmake_minimum_required(VERSION 3.2)

ADD_CUSTOM_TARGET(run-open-intent-js-integration-test
        DEPENDS open-intent-docker
)

ADD_CUSTOM_COMMAND(TARGET run-open-intent-js-integration-test
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/launch.sh chatbot
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_CUSTOM_COMMAND(TARGET run-open-intent-js-integration-test
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/launch.sh tutorial
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
