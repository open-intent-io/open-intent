
include(CMakeParseArguments)

FUNCTION(COPY_RES_FILES)
    set(options "")
    set(oneValueArgs DEST_DIR SRC_DIR)
    set(multiValueArgs RES_FILES)
    cmake_parse_arguments(COPY_RES_FILES "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    MESSAGE("${COPY_RES_FILES_RES_FILES}")

    FOREACH(SRC_FILE ${COPY_RES_FILES_RES_FILES})
        SET(DEST_FILE ${COPY_RES_FILES_DEST_DIR}/${SRC_FILE})

        configure_file(${COPY_RES_FILES_SRC_DIR}${SRC_FILE} ${DEST_FILE} COPYONLY)

        MESSAGE("Copying ${SRC_FILE} to ${DEST_FILE}")
    ENDFOREACH()

ENDFUNCTION()

FUNCTION(GENERATE_RUN_SCRIPT)

    SET(RUN_SCRIPT "
#!/bin/bash

export LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src

cd ${CMAKE_CURRENT_SOURCE_DIR}
nodejs with_json_model.js
    ")

    set(options "")
    set(oneValueArgs SCRIPT_NAME DESTINATION)
    set(multiValueArgs "")
    cmake_parse_arguments(GENERATE_RUN_SCRIPT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    FILE(WRITE ${GENERATE_RUN_SCRIPT_SCRIPT_NAME} ${RUN_SCRIPT})

    FILE(INSTALL ${GENERATE_RUN_SCRIPT_SCRIPT_NAME} DESTINATION ${GENERATE_RUN_SCRIPT_DESTINATION}
            FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
ENDFUNCTION()
