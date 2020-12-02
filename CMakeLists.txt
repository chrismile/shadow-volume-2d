cmake_minimum_required (VERSION 3.11)
project (sv2d)
#MESSAGE(STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})

set (CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
IF(WIN32)
	add_executable(sv2d WIN32 ${SOURCES})
ELSE()
	add_executable(sv2d ${SOURCES})
ENDIF()
include_directories(src)

#make VERBOSE=1

IF(WIN32)
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
	target_link_libraries(sv2d mingw32 SDL2main SDL2)
ELSE()
	target_link_libraries(sv2d SDL2)
ENDIF()

cmake_policy(SET CMP0012 NEW)
find_package(sgl REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(sv2d sgl ${Boost_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
include_directories(${sgl_INCLUDES} ${Boost_INCLUDES} ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDES})
