set(ProjectId TestParallelVector5)
project(${ProjectId})

add_executable(${ProjectId} main.cpp testParallelVector5.cpp)
target_link_libraries(${ProjectId} containers gtest_main)
