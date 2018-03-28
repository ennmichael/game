cmake_minimum_required(VERSION 3.1.0)

project(spacker)
add_executable(spacker src/main.cpp src/spacker.cpp)

find_package(PNG)

if (PNG_FOUND)
        target_link_libraries(spacker ${PNG_LIBRARIES})
else()
        message(WARNING "Png not found")
endif()

add_compile_options(spacker)
target_include_directories(spacker PRIVATE include/spacker)

