cmake_minimum_required(VERSION 3.0.2)

project(sdl++)
add_library(sdl++ STATIC src/sdl++.cpp)

target_include_directories(sdl++ PUBLIC "${sdl++_SOURCE_DIR}/include")
target_include_directories(sdl++ PRIVATE "${sdl++_SOURCE_DIR}/include/sdl++")

target_compile_options(sdl++ PRIVATE "-Wall")
target_compile_options(sdl++ PRIVATE "-Wextra")
target_compile_options(sdl++ PRIVATE "-std=c++17")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${sdl++_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

target_include_directories(sdl++ PRIVATE ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR})
target_link_libraries(sdl++ ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES})

