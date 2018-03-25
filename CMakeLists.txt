cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(game)

# These lines are commented while I work on spacker
#add_executable(game src/main.cpp src/mike.cpp src/mike_sprite.cpp)

#target_include_directories(game PRIVATE "${game_SOURCE_DIR}/include/game")

macro(add_compile_options target)
        target_compile_options(${target} PRIVATE "-Wall")
        target_compile_options(${target} PRIVATE "-Wextra")
        target_compile_options(${target} PRIVATE "-std=c++17")
endmacro()

#add_compile_options(game)

add_subdirectory(engine)
#target_link_libraries(game engine)

add_subdirectory(spacker) # Optionally compile this

