add_executable(test_libifj20c ../apps/test_main.c)

target_include_directories(test_libifj20c PUBLIC ../include/test_libifj20c)
target_link_libraries(test_libifj20c PUBLIC libifj20c)
