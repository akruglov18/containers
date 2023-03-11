set(ProjectId containers)
project(${ProjectId})

file(GLOB ALL_SOURCE_FILES *.cpp *.h *.hpp)

add_library(${ProjectId} ${ALL_SOURCE_FILES})
target_include_directories(${ProjectId} PUBLIC .)
