cmake_minimum_required(VERSION 3.1.0)

project(sdl++)
add_library(sdl++ STATIC src/sdl++.cpp)

target_include_directories(sdl++ PUBLIC "${sdl++_SOURCE_DIR}/include")
target_include_directories(sdl++ PRIVATE "${sdl++_SOURCE_DIR}/include/sdl++")

add_compile_options(sdl++)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${sdl++_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

if (${SDL2_FOUND} STREQUAL FALSE)
        message(FATAL_ERROR "SDL2 not found.")
endif()

if (${SDL2_IMAGE_FOUND} STREQUAL FALSE)
        message(FATAL_ERROR "SDL2_image not found.")
endif()

target_include_directories(sdl++ PRIVATE ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR})
target_link_libraries(sdl++ ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES})

