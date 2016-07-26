cmake_minimum_required(VERSION 3.2)

ADD_CUSTOM_TARGET(run-nodejs-integration-tests)

ADD_CUSTOM_COMMAND(TARGET run-nodejs-integration-tests
        DEPENDS nodejs-binding-build
        COMMAND launch.sh rest-chatbot
        WORKING_DIRECTORY .
)

ADD_CUSTOM_COMMAND(TARGET run-nodejs-integration-tests
        DEPENDS nodejs-binding-build
        COMMAND launch.sh tutorial
        WORKING_DIRECTORY .
)
