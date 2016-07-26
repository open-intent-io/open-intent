cmake_minimum_required(VERSION 3.2)

FIND_PROGRAM( DOCKER_PATH docker )

IF(DOCKER_PATH)
    ADD_CUSTOM_TARGET(open-intent-docker
            DEPENDS open-intent-js
    )
    ADD_CUSTOM_COMMAND(TARGET open-intent-docker POST_BUILD
            COMMAND docker build -t open-intent .
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    ADD_SUBDIRECTORY(test-integration)
ELSE()
    MESSAGE(WARNING "Docker has not been found. Please note that you will not be able to execute the integration tests.")
ENDIF()

ADD_CUSTOM_TARGET(run-open-intent-js-test
        DEPENDS open-intent-js
        COMMAND npm test
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)