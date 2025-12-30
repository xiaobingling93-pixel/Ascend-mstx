set(gtest_header ${THIRDPARTY_PATH}/googletest/googletest/include/)
set(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage -lgcov")
set(CMAKE_C_FLAGS "-fprofile-arcs -ftest-coverage -lgcov")

add_executable(mstx_test_${LANG} test_core_api.c)
add_library(mstx_static_lib_${LANG} STATIC test_core.cpp)


if ("${LANG}" STREQUAL "C")
    add_definitions(-DLANG_C_TEST)
    set(CMAKE_C_COMPILER "gcc")
else()
    set(CMAKE_C_COMPILER "g++")
endif()



target_include_directories(mstx_test_${LANG} PUBLIC ${gtest_header} ${MSTX_HEADER})
target_include_directories(mstx_static_lib_${LANG} PUBLIC ${gtest_header} ${MSTX_HEADER})
target_link_libraries(mstx_test_${LANG} gtest_main mstx_static_lib_${LANG} dl pthread)

add_library(injection_test SHARED injection_stub.cpp)

target_include_directories(injection_test PUBLIC ${gtest_header} ${MSTX_HEADER})

add_executable(mstx_test_lib_${LANG} test_c_cpp_lib.cpp)

if ("${LANG}" STREQUAL "C")
    add_definitions(-DLANG_C_TEST)
    set(CMAKE_C_COMPILER "gcc")
else()
    set(CMAKE_C_COMPILER "g++")
endif()

target_include_directories(mstx_test_lib_${LANG} PUBLIC ${gtest_header})
target_link_libraries(mstx_test_lib_${LANG} gtest_main dl pthread)
