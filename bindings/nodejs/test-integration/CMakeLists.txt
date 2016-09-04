cmake_minimum_required(VERSION 3.2)

ADD_CUSTOM_TARGET(build-integration-test-docker
        COMMAND docker build -t open-intent-integration ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS open-intent-docker
)

ADD_CUSTOM_TARGET(run-integration-tests-js
        DEPENDS run-integration-tests-js-chatbot
                run-integration-tests-js-tutorial
                run-integration-tests-js-elasticsearch-logs
)

ADD_CUSTOM_TARGET(run-integration-tests-js-chatbot
        DEPENDS build-integration-test-docker
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/chatbot/launch.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_CUSTOM_TARGET(run-integration-tests-js-tutorial
        DEPENDS build-integration-test-docker
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tutorial/launch.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_CUSTOM_TARGET(run-integration-tests-js-elasticsearch-logs
        DEPENDS build-integration-test-docker
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/elasticsearch-logs/launch.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)