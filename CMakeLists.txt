cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_COMPILER "g++")
project(mstx VERSION 1.0.0)

get_filename_component(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
get_filename_component(THIRDPARTY_PATH thirdparty/ ABSOLUTE)
get_filename_component(MSTX_HEADER c/include/ ABSOLUTE)

set(CMAKE_CXX_FLAGS "-std=c++11")

# 检查是否可用 ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
else()
    message(STATUS "ccache not found, using native compiler.")
endif()

# 检查是否可用 python
find_program(PYTHON NAMES python3 python)
if(NOT PYTHON)
    message(FATAL_ERROR "Could not find python3 or python in PATH!")
else()
    message(STATUS "Found Python: ${PYTHON}")
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/output CACHE STRING "install prefix" FORCE)
    message(STATUS "No install prefix selected, default to ${CMAKE_INSTALL_PREFIX}")
endif() 

# never install gtest to output path
set(INSTALL_GTEST OFF)
if (${BUILD_TESTS})
    # build test
    message(STATUS "Enable unit tests build")
    add_subdirectory(thirdparty/googletest EXCLUDE_FROM_ALL)
    add_subdirectory(test/c)
endif()

#build so
include(cmake/options.cmake)
add_subdirectory(c)
add_subdirectory(python)

set(PACKAGE_DIR ${CMAKE_CURRENT_BINARY_DIR}/package/mstx)
add_custom_command(
    OUTPUT ${PACKAGE_DIR}/package.flag
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}/lib64 ${CMAKE_INSTALL_PREFIX}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ROOT_DIR}/c/include/mstx ${PACKAGE_DIR}/include
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:mstx> $<TARGET_FILE:ms_tools_ext> ${PACKAGE_DIR}/lib64
    COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_DIR}/README.md ${PACKAGE_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_DIR}/setup.py ${PACKAGE_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_DIR}/MANIFEST.in ${PACKAGE_DIR}/
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_INSTALL_PREFIX}/mstx*.whl
    COMMAND cd ${PACKAGE_DIR} && ${PYTHON} setup.py bdist_wheel --dist-dir ${CMAKE_INSTALL_PREFIX}/
    COMMAND chmod 550 ${CMAKE_INSTALL_PREFIX}/*.whl
    COMMAND touch ${PACKAGE_DIR}/package.flag
    COMMENT "Copy mstx include folder and so to output dir, and make mstx whl package"
)

add_custom_target(package_mstx ALL DEPENDS
                    ms_tools_ext
                    mstx
                    ${ROOT_DIR}/MANIFEST.in
                    ${ROOT_DIR}/README.md
                    ${ROOT_DIR}/setup.py
                    ${PACKAGE_DIR}/package.flag)
