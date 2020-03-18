project(test_surface_distance LANGUAGES CXX)

add_executable(test_surface_distance
    "main.cpp"
    "distance.cpp"
)

target_link_libraries(test_surface_distance PRIVATE surface_distance_lib)
target_include_directories(test_surface_distance PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME test_surface_distance COMMAND test_surface_distance)