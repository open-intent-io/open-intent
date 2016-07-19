cmake_minimum_required(VERSION 3.2)

PROJECT(intent)

set(VERSION "1.0.0")

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-reorder")
elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wno-reorder")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(Custom)


option(BINDINGS_ENABLED "Builds the bindings" ON)
option(GCOV_ENABLED "Enable gcov for test coverage" OFF)

ADD_SUBDIRECTORY(bindings)

ADD_SUBDIRECTORY(examples)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(thirdparty)


find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doxygen
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM
            )
    add_custom_command(TARGET doxygen POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan
            "Open file://${CMAKE_CURRENT_BINARY_DIR}/html/index.html")
endif(DOXYGEN_FOUND)

ADD_CUSTOM_TARGET(run-all-tests
        DEPENDS domotic-test
        intent-test
        beerbar-test
        run-nodejs-tests
        run-nodejs-integration-tests)

ADD_CUSTOM_COMMAND(TARGET run-all-tests POST_BUILD
        COMMAND ${CMAKE_BINARY_DIR}/examples/Domotic/test/domotic-test
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/examples/Domotic/test
)

ADD_CUSTOM_COMMAND(TARGET run-all-tests POST_BUILD
        COMMAND ${CMAKE_BINARY_DIR}/examples/BeerBar/test/beerbar-test
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/examples/BeerBar/test
        )

ADD_CUSTOM_COMMAND(TARGET run-all-tests POST_BUILD
        COMMAND ${CMAKE_BINARY_DIR}/test/tests/intent-test
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/tests/
)