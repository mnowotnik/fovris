
set(EXAMPLES
    deserializer.cpp
    implicit_api.cpp
    )

foreach( example ${EXAMPLES} )
    add_executable(${example} ${example})
    target_link_libraries(${example} libfovris_shared)
    set_target_properties(
        ${example}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples"
    )
endforeach( example ${EXAMPLES} )

