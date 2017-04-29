function(DEFINE_TESTS
         TESTS
         LIBS)
    set(ALL_TEST_SRCS)
    foreach(TEST ${TESTS})
        message("ADDING TEST ${TEST}")
        get_filename_component(TEST_BASENAME ${TEST} NAME)
        get_filename_component(TEST_DIRNAME ${TEST} DIRECTORY)
        if(NOT "${TEST_DIRNAME}" STREQUAL "")
            set(TEST_DIRNAME "${TEST_DIRNAME}/")
        endif()
        string(REGEX REPLACE "/" "_" TEST_TARGET "test_${TEST_DIRNAME}${TEST_BASENAME}")
        set(TEST_SRC "${TEST_DIRNAME}test_${TEST_BASENAME}.cpp")
        message("TEST_TARGET ${TEST_TARGET}")
        list(APPEND ALL_TEST_SRCS "${TEST_SRC}")
        add_executable(${TEST_TARGET} "${TEST_SRC}")
        target_link_libraries(${TEST_TARGET} ${SYSTEM_LIBRARIES})
        target_link_libraries(${TEST_TARGET} skiff_gtest)
        target_link_libraries(${TEST_TARGET} ${LIBS})
        add_test(${TEST} ${TEST_TARGET})
    endforeach()
    add_executable(test_all ${ALL_TEST_SRCS})
    target_link_libraries(test_all ${SYSTEM_LIBRARIES})
    target_link_libraries(test_all skiff_gtest)
    target_link_libraries(test_all ${LIBS})
endfunction()
