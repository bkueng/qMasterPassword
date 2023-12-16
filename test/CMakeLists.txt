if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_sources(${TARGET} PRIVATE test.cpp)

    target_compile_definitions(${TARGET} PRIVATE TESTING_SUPPORT _DEBUG)

    target_link_libraries(${TARGET} PRIVATE Qt::Test)

    add_custom_target(
        test
        COMMAND ${TARGET} --test test/tests.xml
        DEPENDS ${TARGET}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Running tests...")
endif()
