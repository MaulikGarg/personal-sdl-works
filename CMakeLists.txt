cmake_minimum_required(VERSION 3.10)
project(TEST VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the sources
set(SOURCES Playground/test.cpp)

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Use target-specific include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
        ${SDL2_INCLUDE_DIRS} 
        ${SDL2_image_INCLUDE_DIRS} 
        ${SDL2_ttf_INCLUDE_DIRS}
        ${SDL2_mixer_INCLUDE_DIRS}
    )

# Link SDL2 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
        ${SDL2_LIBRARIES} 
        ${SDL2_image_LIBRARIES}
        ${SDL2_ttf_LIBRARIES}
        ${SDL2_mixer_LIBRARIES}
    )

# MinGW-specific linking
if(MINGW)

    target_link_libraries(${PROJECT_NAME} PRIVATE 
        mingw32 
        SDL2::SDL2main 
        SDL2::SDL2 
        -lSDL2_image
        -lSDL2_ttf
        -lSDL2_mixer)

endif()
