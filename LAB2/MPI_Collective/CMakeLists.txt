cmake_minimum_required(VERSION 3.28.3)
set (LINUX_HEADERS_PATH "/usr/src/linux-hwe-6.14-headers-6.14.0-35/include/")
project (TEST)
find_package(MPI)

add_executable (main  main/main.cpp)
if(MPI_FOUND)
    target_link_libraries(main PRIVATE MPI::MPI_CXX) # For C++
endif()
target_include_directories(main PRIVATE ${LINUX_HEADERS_PATH})
