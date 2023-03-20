set(ProjectId TestParallelVector4)
project(${ProjectId})

add_executable(${ProjectId} main.cpp testParallelVector4.cpp)
target_link_libraries(${ProjectId} containers gtest_main)
