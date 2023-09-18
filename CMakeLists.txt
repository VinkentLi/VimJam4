cmake_minimum_required(VERSION 3.7)
project(Paths)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2_image REQUIRED CONFIG REQUIRED COMPONENTS SDL2_image)
find_package(SDL2_mixer REQUIRED CONFIG REQUIRED COMPONENTS SDL2_mixer)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

# set(SDL2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/cmake/SDL2)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${INCLUDE_DIR})

file(GLOB_RECURSE HEADER_FILES ${INCLUDE_DIR}/*.h)
file(GLOB_RECURSE SOURCE_FILES ${SOURCE_DIR}/*.cpp)

add_executable(Paths ${HEADER_FILES} ${SOURCE_FILES})

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose build type (Debug or Release): ")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -g -O0)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -O2 -mwindows)
endif()

if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    target_link_libraries(${PROJECT_NAME} PRIVATE -lmingw32)
endif()

if (TARGET SDL::SDL2main)
    target_link_libraries(Paths PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(Paths PRIVATE SDL2::SDL2)
target_link_libraries(Paths PRIVATE SDL2_image::SDL2_image)
target_link_libraries(Paths PRIVATE SDL2_mixer::SDL2_mixer)
