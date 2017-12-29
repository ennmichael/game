cmake_minimum_required(VERSION 3.1.0)

project(game)
add_executable(game src/main.cpp)

macro(add_compile_options target)
        target_compile_options(${target} PRIVATE "-Wall")
        target_compile_options(${target} PRIVATE "-Wextra")
        target_compile_options(${target} PRIVATE "-std=c++17")
endmacro()

add_compile_options(game)

add_subdirectory(engine)
target_link_libraries(game engine)

