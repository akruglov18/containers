set(ProjectId testStdVector)
project(${ProjectId})

add_executable(${ProjectId} main.cpp testStdVector.cpp)
target_link_libraries(${ProjectId} containers gtest_main)
