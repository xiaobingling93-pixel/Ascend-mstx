add_library(ms_tools_ext SHARED ms_tools_ext.cpp)
target_include_directories(ms_tools_ext PUBLIC ${MSTX_HEADER})
target_link_libraries(ms_tools_ext dl)
install(TARGETS ms_tools_ext
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/mstx/lib64)