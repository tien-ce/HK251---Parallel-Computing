cmake_minimum_required(VERSION 3.28.3)
project(parallel CXX)
# Use GLOB_RECURSE to search recursively (due to the ** pattern)
# Step 1: Use GLOB to find the top-level component *directories*.
# Note: We set LIST_DIRECTORIES TRUE and use a non-recursive pattern to get 
#       just the immediate component directories (e.g., .../PDE, .../Read_Write_Matrix).
file(GLOB COMPONENT_BASE_DIRS
    LIST_DIRECTORIES TRUE
    # Non-recursive glob to match PDE and Read_Write_Matrix folders
    "../..//Components/**" 
)

#Execualbe target
add_executable(parallel main.cpp)

# 1. Include Directories
foreach(DIR ${COMPONENT_BASE_DIRS})
    # Append the "Include" subdirectory path to our final list
    target_include_directories(parallel 
    PRIVATE "${DIR}/Include" 
    )
endforeach()

# 2. Link Directories
foreach(DIR ${COMPONENT_BASE_DIRS})
    # Append the "Include" subdirectory path to our final list
    target_link_directories(parallel 
        PRIVATE "${DIR}/build"
    )
endforeach()

target_link_libraries(parallel PRIVATE read_write_matrix blast_caculator work_pool)
