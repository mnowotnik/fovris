project (lemon)
cmake_minimum_required(VERSION 2.8)
add_executable(lemon lemon.c)

add_custom_command(
    TARGET lemon
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/lempar.c" "${CMAKE_CURRENT_BINARY_DIR}"
)
