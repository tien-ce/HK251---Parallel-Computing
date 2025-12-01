cmake_minimum_required(VERSION 3.28.3)
project(WORK_POOL CXX) # Use Cpp compiler
add_library (work_pool STATIC work_pool.cpp)  #Create a static libaray named work_pool
target_include_directories(work_pool PRIVATE Include)
# Add the directory for finding the header files