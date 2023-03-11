set(ProjectId testParallelVector2)
project(${ProjectId})

add_executable(${ProjectId} main.cpp testParallelVector2.cpp)
target_link_libraries(${ProjectId} containers gtest_main)
