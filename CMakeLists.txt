# IFJ PROJECT 2020
# CMAKE project structure adapted from
# https://gitlab.com/CLIUtils/modern-cmake/-/tree/master/examples/extended-project
cmake_minimum_required(VERSION 3.12)

project(ifj20c LANGUAGES C)
set(CMAKE_C_STANDARD 99)

add_subdirectory(src)
add_subdirectory(tests)

add_executable(${PROJECT_NAME} apps/main.c)
target_link_libraries(${PROJECT_NAME} PUBLIC libifj20c)