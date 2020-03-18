cmake_minimum_required(VERSION 3.0.0)
project(surface-distance VERSION 0.1.0)


add_library(surface_distance_lib
    "distance.cpp"
)

target_compile_features(surface_distance_lib PUBLIC cxx_std_14)
target_include_directories(surface_distance_lib 
                            PUBLIC ${PROJECT_SOURCE_DIR} 
							PUBLIC lib)

add_executable(surface_distance_exe
    "main.cpp"
)

set(resource_file
    "st-helens/pre.data" 
	"st-helens/post.data"
)

foreach(file ${resource_file})
	configure_file(${file} ${PROJECT_BINARY_DIR} COPYONLY)
endforeach()



target_link_libraries(surface_distance_exe PRIVATE surface_distance_lib)
