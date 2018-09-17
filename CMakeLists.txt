cmake_minimum_required (VERSION 2.6)
project (sv2d)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
#MESSAGE(STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
add_executable(sv2d ${SOURCES})
include_directories(src)

#make VERBOSE=1

find_package(SGL REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
target_link_libraries(sv2d GL)
target_link_libraries(sv2d GLEW)
target_link_libraries(sv2d sgl)
