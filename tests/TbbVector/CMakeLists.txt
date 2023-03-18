set(ProjectId TestTbbVector)
project( ${ProjectId} )

file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)

add_executable( ${ProjectId} ${ALL_SOURCE_FILES} )

target_link_libraries(${ProjectId} ${TBB_IMPORTED_TARGETS})
target_link_libraries(${ProjectId} gtest gtest_main)

