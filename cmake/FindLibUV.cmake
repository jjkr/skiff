# - Try to find libuv
# Once done, this will define
#
#  LIBUV_FOUND - system has libuv
#  LIBUV_INCLUDE_DIRS - the libuv include directories
#  LIBUV_LIBRARIES - link these to use libuv
#
# Set the LIBUV_USE_STATIC variable to specify if static libraries should
# be preferred to shared ones.

FIND_PATH(LIBUV_INCLUDE_DIR NAMES uv.h)
FIND_LIBRARY(LIBUV_LIBRARIES NAMES uv.a libuv.a)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBUV DEFAULT_MSG LIBUV_LIBRARIES LIBUV_INCLUDE_DIR)
