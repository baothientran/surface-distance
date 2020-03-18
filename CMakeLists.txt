cmake_minimum_required(VERSION 3.5)

project(surface_distance LANGUAGES CXX)

add_subdirectory(src)
add_subdirectory(test)
add_test(NAME test_surface_distance COMMAND test_surface_distance)
