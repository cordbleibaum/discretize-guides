cmake_minimum_required(VERSION 3.20)
project(benchfactor-2021)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# benchfactor-2021
add_executable(${PROJECT_NAME} 
    application/main.cpp
    benchmarks/ram_stream.cpp
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /arch:AVX2 /openmp)
endif()

# json
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json nlohmann_json::nlohmann_json)

# glad
find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)

# glfw
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# openmp
find_package(OpenMP REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)

# wmi
target_link_libraries(${PROJECT_NAME} PRIVATE wbemuuid)

# Boost
find_package(Boost REQUIRED COMPONENTS)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::boost)
