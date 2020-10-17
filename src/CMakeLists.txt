file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/libifj20c/*.h")

add_library(libifj20c scanner.c token.c ${HEADER_LIST})

target_include_directories(libifj20c PUBLIC ../include/libifj20c)

source_group(
        TREE "${PROJECT_SOURCE_DIR}/include/libifj20c"
        PREFIX "Header Files"
        FILES ${HEADER_LIST})
