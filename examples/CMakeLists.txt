cmake_minimum_required(VERSION 3.2)

ADD_SUBDIRECTORY(BeerBar)
ADD_SUBDIRECTORY(QueriesBar)
ADD_SUBDIRECTORY(Domotic)
ADD_SUBDIRECTORY(IntentViz)

ADD_CUSTOM_TARGET(run-integration-tests-cpp
        DEPENDS run-integration-tests-cpp-domotic run-integration-tests-cpp-beerbar
)

ADD_CUSTOM_TARGET(run-integration-tests-cpp-domotic
        DEPENDS domotic-test
        COMMAND ${CMAKE_BINARY_DIR}/examples/Domotic/test/domotic-test
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/examples/Domotic/test
)

ADD_CUSTOM_TARGET(run-integration-tests-cpp-beerbar
        DEPENDS beerbar-test
        COMMAND ${CMAKE_BINARY_DIR}/examples/BeerBar/test/beerbar-test
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/examples/BeerBar/test
)