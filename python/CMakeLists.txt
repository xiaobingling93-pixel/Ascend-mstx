find_package(Python REQUIRED COMPONENTS Development)
message(STATUS "Found Python: ${Python_VERSION}")
message(STATUS "Python include directories: ${Python_INCLUDE_DIRS}")
message(STATUS "Python libraries: ${Python_LIBRARIES}")

add_library(mstx SHARED "mstx_api.cpp"
                        "mstx_methods.cpp"
                        "py_init_mstx.cpp")
target_include_directories(mstx PUBLIC ${MSTX_HEADER} ${Python_INCLUDE_DIRS})
set_target_properties(mstx PROPERTIES PREFIX  "")
target_link_libraries(mstx dl)
install(TARGETS mstx LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/mstx/lib64)
