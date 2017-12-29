cmake_minimum_required(VERSION 3.1.0)

project(engine)
add_library(engine STATIC src/graphics.cpp src/config_loader.cpp src/gameplay.cpp)

target_include_directories(engine PUBLIC "${engine_SOURCE_DIR}/include")
target_include_directories(engine PRIVATE "${engine_SOURCE_DIR}/include/engine")

add_compile_options(engine)

add_subdirectory(sdl++)
target_link_libraries(engine sdl++)

