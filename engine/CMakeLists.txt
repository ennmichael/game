cmake_minimum_required(VERSION 3.1.0)

project(engine)
add_library(engine STATIC src/graphics.cpp src/gameplay.cpp src/keyboard.cpp src/sdl++.cpp)

target_include_directories(engine PUBLIC "${engine_SOURCE_DIR}/include")
target_include_directories(engine PRIVATE "${engine_SOURCE_DIR}/include/engine")

add_compile_options(engine)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${engine_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

if (SDL2_FOUND)
        target_include_directories(engine PRIVATE ${SDL2_INCLUDE_DIR})
        target_link_libraries(engine PRIVATE ${SDL2_LIBRARY})
else()
        message(WARNING "SDL2 not found.")
endif()

if (SDL2_IMAGE_FOUND)
        target_include_directories(engine PRIVATE ${SDL2_IMAGE_INCLUDE_DIR})
        target_link_libraries(engine PRIVATE ${SDL2_IMAGE_LIBRARIES})
else()
        message(WARNING "SDL2_image not found.")
endif()

