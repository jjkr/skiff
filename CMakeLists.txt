cmake_minimum_required(VERSION 3.0)
project(skiff C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")

find_package(Threads REQUIRED)
find_package(LibUV REQUIRED)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs all)
set(SYSTEM_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} ${llvm_libs})

include_directories(SYSTEM "${LIBUV_INCLUDE_DIR}")
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
include_directories("${CMAKE_SOURCE_DIR}/src" SYSTEM "${CMAKE_SOURCE_DIR}/vend")
add_definitions(${LLVM_DEFINITIONS})

add_subdirectory(src)

add_subdirectory(examples)

include_directories("${CMAKE_SOURCE_DIR}")
enable_testing()
include(${CMAKE_SOURCE_DIR}/cmake/UnitTest.cmake)
add_subdirectory(test)
