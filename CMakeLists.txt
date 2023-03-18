cmake_minimum_required(VERSION 3.12)

include("${CMAKE_SOURCE_DIR}/cmake/Configure.cmake")

SET(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
include_directories("${CMAKE_SOURCE_DIR}/3rdparty/gtest/include")
include_directories("${CMAKE_SOURCE_DIR}/3rdparty/gtest/googletest/include")
add_subdirectory("${CMAKE_SOURCE_DIR}/3rdparty/gtest")

include( cmake/TBBGet.cmake )
tbb_get( TBB_ROOT tbb_root RELEASE_TAG "v2020.3" CONFIG_DIR TBB_DIR )

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SYSTEM_BIT "intel64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(SYSTEM_BIT "ia32")
endif()

file(GLOB TBB_PRE_LIB  "${tbb_root}/bin/${SYSTEM_BIT}/vc14/*.dll")
file(COPY ${TBB_PRE_LIB} DESTINATION "${CMAKE_BINARY_DIR}/bin")

find_package(TBB)

if(TBB_FOUND)
    message(STATUS "Found TBB: TRUE (found version \"${TBB_VERSION_MAJOR}.${TBB_VERSION_MINOR}\") ")
    include_directories("${tbb_root}/include")
endif(TBB_FOUND)

add_subdirectory(containers)
add_subdirectory(tests)
