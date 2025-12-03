cmake_minimum_required (VERSION 3.28.3)
project (blast_caculator CXX)
add_library (blast_caculator STATIC blast_caculator.cpp)
target_include_directories(blast_caculator PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Include")