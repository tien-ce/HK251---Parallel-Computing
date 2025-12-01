cmake_minimum_required(VERSION 3.28.3)
project(hello_workPool CXX)
#Execualbe target
add_executable(hello_workPool main.cpp)

# Add the directory for finding the header files
target_include_directories(hello_workPool PRIVATE ../Components/Simple_workPool/Include)

# Add the directory for finding the static libary
target_link_directories(hello_workPool PRIVATE ../Components/Simple_workPool/build)

# Add the library to link
target_link_libraries(hello_workPool PRIVATE work_pool)