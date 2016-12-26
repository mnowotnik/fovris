
option(USE_GCOV "Enable gcov support" OFF)

if(USE_GCOV)
    message(STATUS "Enabling gcov support")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
endif()

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/)

add_custom_command (
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/QlParse.cc ${CMAKE_CURRENT_SOURCE_DIR}/QlParse.h
    COMMAND lemon -s ${CMAKE_CURRENT_SOURCE_DIR}/QlParse.y
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/QlParse.c ${CMAKE_CURRENT_SOURCE_DIR}/QlParse.cc
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/QlParse.y
    DEPENDS lemon
  )

include(KB/CMakeLists.txt)
include(Eval/CMakeLists.txt)
include(Pred/CMakeLists.txt)

set(SRC
    DateTime.cc
    Evaluation.cc
    GroundLiteral.cc
    KnowledgeBase.cc
    Lexer.cc
    Literal.cc
    Module.cc
    NaiveEvaluation.cc
    Program.cc
    QlDeserializer.cc
    QlParse.cc
    QlScanner.re2c.cc
    Query.cc
    QueryResult.cc
    QueryResultPrinter.cc
    RelationDef.cc
    ResultFact.cc
    Rule.cc
    RuleDisjunct.cc
    RuleLiteral.cc
    SemiNaiveEvaluation.cc
    Term.cc
    TermMapper.cc
    TruthValue.cc
    TruthValueSet.cc
    Utils.cc
    )

find_program(RE2C_EXECUTABLE re2c)
if(NOT RE2C_EXECUTABLE STREQUAL "RE2C_EXECUTABLE-NOTFOUND")
    add_custom_command (
        OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/QlScanner.re2c.cc"
        COMMAND re2c -i -o QlScanner.re2c.cc QlScanner.re 
        DEPENDS QlScanner.re
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()

set(LIB_SRC
    ${SRC}
    ${KB_SRC}
    ${EVAL_SRC}
    ${Pred_SRC}
    )

add_library(libfovris_shared SHARED ${LIB_SRC})

install(TARGETS libfovris_shared 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/fovris
    )

install(DIRECTORY ${INCLUDE_DIR}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fovris)

set_target_properties(libfovris_shared PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  OUTPUT_NAME fovris
  )
