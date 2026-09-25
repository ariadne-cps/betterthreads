set(UNIT_TESTS
    test_buffer
    test_buffered_thread
    test_thread
    test_thread_pool
    test_thread_manager
    test_workload_advancement
    test_workload
)

foreach(TEST ${UNIT_TESTS})
    add_executable(${TEST} ${TEST}.cpp)
    target_link_libraries(${TEST} threading)
    add_test(NAME ${TEST} COMMAND $<TARGET_FILE:${TEST}>)
    if(WIN32)
        set_tests_properties(${TEST} PROPERTIES
            ENVIRONMENT_MODIFICATION
                "PATH=path_list_prepend:$<TARGET_FILE_DIR:threading>"
                "PATH=path_list_prepend:$<TARGET_FILE_DIR:logging>"
                "PATH=path_list_prepend:$<TARGET_FILE_DIR:helper>"
            TIMEOUT 120
        )
    endif()
endforeach()

add_custom_target(tests)
add_dependencies(tests ${UNIT_TESTS})
