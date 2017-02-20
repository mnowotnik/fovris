set(LINENOISE_PATH
    "${PROJECT_SOURCE_DIR}/thirdparty/linenoise-ng"
    )
set(CMDLINE_PARSER_PATH
    "${PROJECT_SOURCE_DIR}/thirdparty/optionparser"
    )

add_custom_command (
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/CmdLineScanner.re2c.cc"
    COMMAND re2c -i -o CmdLineScanner.re2c.cc CmdLineScanner.re 
    DEPENDS CmdLineScanner.re
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

set(HEADERS
    CmdLineScanner.h
    )

set(SRC
    CmdLineScanner.re2c.cc
    Cli.cc)

include_directories(
    "${LINENOISE_PATH}/include"
    "${CMDLINE_PARSER_PATH}/include"
    )

add_executable(fovris ${SRC} ${HEADERS})
target_link_libraries(fovris libfovris_shared linenoise)

install(TARGETS fovris 
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
