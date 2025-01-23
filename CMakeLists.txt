cmake_minimum_required(VERSION 3.9)
project(PageRank)

set(CMAKE_CXX_STANDARD 17)

# Find OpenMP
find_package(OpenMP REQUIRED)

# Add executable
add_executable(PageRank main.cpp)

# Explicit OpenMP linking for MinGW
if(MINGW)
    target_link_libraries(PageRank PRIVATE OpenMP::OpenMP_CXX -fopenmp)
endif()

# Ensure OpenMP compilation flags
set_target_properties(PageRank PROPERTIES
        COMPILE_FLAGS "-fopenmp"
        LINK_FLAGS "-fopenmp"
)