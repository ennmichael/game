cmake_minimum_required(VERSION 3.1.0)

project(game)
add_executable(game src/main.cpp)

target_compile_options(game PRIVATE "-Wall")
target_compile_options(game PRIVATE "-Wextra")
target_compile_options(game PRIVATE "-std=c++17")

add_subdirectory(engine)
target_link_libraries(game engine)

