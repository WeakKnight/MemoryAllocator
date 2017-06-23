cmake_minimum_required(VERSION 3.0.0)
project(HeapManager VERSION 0.0.0)

SET(CMAKE_C_COMPILER "clang")
SET(CMAKE_CXX_COMPILER "clang++")
SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(CTest)
enable_testing()

add_library(HeapManager HeapAllocator.cpp BitArray.cpp FixedSizeAllocator.cpp HeapManager.cpp Array.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

link_directories( ${CMAKE_CURRENT_BINARY_DIR} )
add_executable(TestProgram main.cpp)
TARGET_LINK_LIBRARIES(TestProgram libHeapManager.a)

