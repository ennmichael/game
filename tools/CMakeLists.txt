cmake_minimum_required(VERSION 3.1.0)

project(tools)
add_executable(add-frame-data src/add_frame_data.cpp)

target_compile_options(add-frame-data PRIVATE "-Wall")
target_compile_options(add-frame-data PRIVATE "-Wextra")
target_compile_options(add-frame-data PRIVATE "-std=c++17")

add_subdirectory("${tools_SOURCE_DIR}/../png++" bin)

target_link_libraries(add-frame-data png++)

