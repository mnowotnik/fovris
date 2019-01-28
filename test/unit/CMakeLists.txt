
set(TEST_SRC
    AllTests.cc
    Eval/EvalRuleTest.cc
    Eval/EvalSubgoalTest.cc
    Eval/ProjectionTest.cc
    Eval/JoinProjectionTest.cc
    Eval/JoinTest.cc
    TestDateTime.cc
)

include_directories(
    "${PROJECT_SOURCE_DIR}/test/unit"
    "${PROJECT_SOURCE_DIR}/include/fovris"
)

add_executable(alltests EXCLUDE_FROM_ALL ${TEST_SRC})
target_link_libraries(alltests libfovris)
