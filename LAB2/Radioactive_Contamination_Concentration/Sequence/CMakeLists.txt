cmake_minimum_required (VERSION 3.28.3)
# Use GLOB_RECURSE to search recursively (due to the ** pattern)
# Step 1: Use GLOB to find the top-level component *directories*.
# Note: We set LIST_DIRECTORIES TRUE and use a non-recursive pattern to get 
#       just the immediate component directories (e.g., .../PDE, .../Read_Write_Matrix).
file(GLOB COMPONENT_BASE_DIRS
    LIST_DIRECTORIES TRUE
    # Non-recursive glob to match PDE and Read_Write_Matrix folders
    "/data200/LAB_Assingment/Parallel_Computing/LAB/LAB2/Components/**" 
)



project(consequence C) # Explicitly specify language for clarity

# Define the executable target
add_executable(consequence main/main.c)

# 1. Include Directories
foreach(DIR ${COMPONENT_BASE_DIRS})
    # Append the "Include" subdirectory path to our final list
    target_include_directories(consequence 
    PRIVATE "${DIR}/Include" 
    )
endforeach()

# 2. Link Directories
foreach(DIR ${COMPONENT_BASE_DIRS})
    # Append the "Include" subdirectory path to our final list
    target_link_directories(consequence 
        PRIVATE "${DIR}/build"
    )
endforeach()


# 3. Link Libraries
target_link_libraries(consequence PRIVATE pde read_write_matrix -lm)

# 4. Find and Link Optional Libraries (MPI)
find_package(MPI)
if(MPI_FOUND)
    # CORRECTED TARGET NAME: Link MPI to 'consequence'
    target_link_libraries(consequence PRIVATE MPI::MPI_C)
    message(STATUS "MPI found and linked to consequence target.")
else()
    message(WARNING "MPI not found. consequence will be built without MPI support.")
endif() 