cmake_minimum_required(VERSION 3.0.0)
project(MemoryAllocator VERSION 0.0.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(CTest)
enable_testing()

add_library(MemoryAllocator HeapAllocator.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

link_directories( ${CMAKE_CURRENT_BINARY_DIR} )
add_executable(TestProgram main.cpp)
TARGET_LINK_LIBRARIES(TestProgram libMemoryAllocator.a)

