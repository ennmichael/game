cmake_minimum_required(VERSION 3.1.0)

project(tools)
add_executable(add-frame-data src/add_frame_data.cpp)

add_subdirectory("${tools_SOURCE_DIR}/../png++" bin)

target_link_libraries(add-frame-data png++)

