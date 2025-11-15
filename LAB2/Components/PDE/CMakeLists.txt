cmake_minimum_required(VERSION 3.28.3)
project(PDE C)  # Use C compiler
add_library(pde STATIC pde.c) # Create static library named pde
target_link_libraries(pde PRIVATE -lm)
target_include_directories(pde PRIVATE Include) # Directory for finding the header files