cmake_minimum_required(VERSION 3.1.0)

project(game)

macro(add_compile_options target)
        target_compile_options(${target} PRIVATE "-Wall")
        target_compile_options(${target} PRIVATE "-Wextra")
        target_compile_options(${target} PRIVATE "-pedantic")
        target_compile_options(${target} PRIVATE "-std=c++17")
endmacro()

if (BUILD_DEV_TOOLS)
        message("Development tools will also be built.")
        add_subdirectory(spacker)
endif()

add_executable(game src/main.cpp src/mike.cpp)
target_include_directories(game PRIVATE "${game_SOURCE_DIR}/include/game")
add_compile_options(game)

add_subdirectory(engine)
target_link_libraries(game engine)

