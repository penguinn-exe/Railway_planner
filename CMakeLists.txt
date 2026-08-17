cmake_minimum_required(VERSION 3.16)
project(RailwayPlanner VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Main Application Executable
add_executable(railway_planner
    src/main.cpp
    src/core/RailwaySystem.cpp
)

# Test Runner Executable
add_executable(run_tests
    tests/test_main.cpp
    src/core/RailwaySystem.cpp
)

enable_testing()
add_test(NAME DSATests COMMAND run_tests)
