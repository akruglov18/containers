set(ProjectId testParallelVector3)
project(${ProjectId})

add_executable(${ProjectId} main.cpp testParallelVector3.cpp)
target_link_libraries(${ProjectId} containers gtest_main)
