cmake_minimum_required(VERSION 3.10)
project(lvgl_test)

# Find SDL2
find_package(SDL2 REQUIRED)

# Add LVGL library
add_subdirectory(${CMAKE_SOURCE_DIR}/.. ${CMAKE_BINARY_DIR}/lvgl)

# Add the test application
add_executable(test_app test_app.c)

# Include directories
target_include_directories(test_app PRIVATE
    ${CMAKE_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/../src
    ${SDL2_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(test_app PRIVATE
    lvgl
    ${SDL2_LIBRARIES}
    m
) 