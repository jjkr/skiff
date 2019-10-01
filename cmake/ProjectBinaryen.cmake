include(ExternalProject)

set(BINARYEN_VERSION version_89)

set(binaryen_prefix "${CMAKE_CURRENT_BINARY_DIR}/binaryen")
set(BINARYEN_LIB_DIR "${binaryen_prefix}/src/binaryen-build/lib")
set(BINARYEN_INCLUDE_DIR "${binaryen_prefix}/src/binaryen/src")

ExternalProject_Add(binaryen
    PREFIX ${binaryen_prefix}
    DOWNLOAD_NAME binaryen-${BINARYEN_VERSION}.tar.gz
    URL https://github.com/WebAssembly/binaryen/archive/${BINARYEN_VERSION}.tar.gz
    URL_HASH SHA256=e8b35e751cc9b90ce4c4a9d309595ee9c3afac2964fd0c4cc06c12ec43f6d55e
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_INSTALL_LIBDIR=lib
    -DCMAKE_BUILD_TYPE=Release
    -DBUILD_STATIC_LIB=ON
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    ${build_command}
    ${install_command}
)

set(binaryen_extra_libs
    ${BINARYEN_LIB_DIR}/libpasses.a
    ${BINARYEN_LIB_DIR}/libwasm.a
    ${BINARYEN_LIB_DIR}/libasmjs.a
    ${BINARYEN_LIB_DIR}/libemscripten-optimizer.a
    ${BINARYEN_LIB_DIR}/libir.a
    ${BINARYEN_LIB_DIR}/libcfg.a
    ${BINARYEN_LIB_DIR}/libsupport.a
)

add_library(binaryen::binaryen STATIC IMPORTED)
set_target_properties(
    binaryen::binaryen
    PROPERTIES
    IMPORTED_CONFIGURATIONS Release
    IMPORTED_LOCATION "${BINARYEN_LIB_DIR}/libbinaryen.a"
    INTERFACE_LINK_LIBRARIES "${binaryen_extra_libs}"
)
add_dependencies(binaryen::binaryen binaryen)
