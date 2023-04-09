set(ProjectId TestParallelVector)
project(${ProjectId})

file(GLOB ALL_SOURCE_FILES *.cpp *.h *.hpp)

add_executable(${ProjectId} main.cpp ${ALL_SOURCE_FILES})
target_link_libraries(${ProjectId} containers gtest_main ${TBB_IMPORTED_TARGETS})
