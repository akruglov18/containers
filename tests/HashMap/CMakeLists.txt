set(ProjectId TestHashMap)
project(${ProjectId})

add_executable(${ProjectId} main.cpp testHashMap.cpp)
target_link_libraries(${ProjectId} containers gtest_main)
