# Google Test
set(GTEST_DIR ${PROJECT_SOURCE_DIR}/third_party/gtest)
include_directories(${GTEST_DIR} ${GTEST_DIR}/include)
add_library(skiff_gtest ${GTEST_DIR}/src/gtest-all.cc ${GTEST_DIR}/src/gtest_main.cc)
set_source_files_properties(${GTEST_DIR}/src/gtest-all.cc PROPERTIES COMPILE_FLAGS -Wno-missing-field-initializers)

set(TESTS
    lexer
    parser
    )

set(LIBS
    skiff
    )

DEFINE_TESTS("${TESTS}" "${LIBS}")

