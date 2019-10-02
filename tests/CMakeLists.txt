enable_testing()

wf_add_test(conftest.cpp)
add_definitions(-DSRCDIR="${PROJECT_SOURCE_DIR}/tests")
