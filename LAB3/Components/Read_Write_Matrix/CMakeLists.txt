cmake_minimum_required (VERSION 3.28.3)
project (read_write_matrix CXX)
add_library (read_write_matrix STATIC read_write_matrix.cpp)
target_include_directories(read_write_matrix PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Include")
