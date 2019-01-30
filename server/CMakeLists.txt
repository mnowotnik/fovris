set(CROW_PATH
    "${PROJECT_SOURCE_DIR}/thirdparty/crow/include"
    )

include_directories(
    ${CROW_PATH}
    ${PROJECT_SOURCE_DIR}/include
    )

set(SRC
    fovrisd.cc)

add_executable(fovrisd ${SRC})
target_link_libraries(fovrisd -lpthread -lm libfovris ${Boost_LIBRARIES})

install(TARGETS fovrisd
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
