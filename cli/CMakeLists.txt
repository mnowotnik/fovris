set(LINENOISE_PATH
    "${PROJECT_SOURCE_DIR}/thirdparty/linenoise-ng"
    )
set(CMDLINE_PARSER_PATH
    "${PROJECT_SOURCE_DIR}/thirdparty/optionparser"
    )

set(HEADERS
    "CmdLineScanner.h"
    "${CMDLINE_PARSER_PATH}/include"
    "${LINENOISE_PATH}/include/linenoise.h")

add_custom_command (
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/CmdLineScanner.re2c.cc"
    COMMAND re2c -i -o CmdLineScanner.re2c.cc CmdLineScanner.re 
    DEPENDS CmdLineScanner.re
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
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
