set(ProjectId testParallelVector)
project(${ProjectId})

add_executable(${ProjectId} main.cpp testParallelVector.cpp)
target_link_libraries(${ProjectId} containers gtest_main)
