cmake_minimum_required(VERSION 3.10)
project(TEST VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# FIND SDL3
find_package(SDL3 REQUIRED)

# set the sources
set(SOURCES
    Playground/test.cpp
)

# Include directories and link libraries
include_directories(${SDL2_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

if(MINGW)
    target_link_libraries(${PROJECT_NAME} mingw32 SDL3)
endif()
