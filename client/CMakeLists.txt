cmake_minimum_required(VERSION 3.20)
project(benchfactor-2021)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

# glad
add_subdirectory(glad)

# openmp
find_package(OpenMP REQUIRED)

# benchfactor-2021
add_executable(${PROJECT_NAME} 
    application/main.cpp
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /arch:AVX2 /openmp)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad OpenMP::OpenMP_CXX)
