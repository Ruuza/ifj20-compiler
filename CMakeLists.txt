# IFJ PROJECT 2020
cmake_minimum_required(VERSION 3.12)

project(ifj20c LANGUAGES C)
set(CMAKE_C_STANDARD 99)

# IFJ20 Library
add_library(libifj20c scanner.c token.c scanner.h token.h)

# IFJ20C Tests
add_executable(test_libifj20c test_main.c)
target_link_libraries(test_libifj20c PUBLIC libifj20c)
# IFJ20C
add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} PUBLIC libifj20c)