cmake_minimum_required(VERSION 3.1.0)

project(png++)
add_library(png++ src/png++.cpp)

find_package(PNG)

if (${PNG_FOUND} STREQUAL FALSE)
        message(FATAL_ERROR "libpng not found.")
endif()

target_include_directories(png++ PRIVATE ${PNG_INCLUDE_DIRS})
target_link_libraries(png++ ${PNG_LIBRARIES})

target_compile_options(png++ PRIVATE "-Wall")
target_compile_options(png++ PRIVATE "-Wextra")
target_compile_options(png++ PRIVATE "-std=c++17")

target_include_directories(png++ PRIVATE "${png++_SOURCE_DIR}/include/png++")
target_include_directories(png++ PUBLIC "${png++_SOURCE_DIR}/include")

