cmake_minimum_required(VERSION 3.0.0)
project(surface-distance VERSION 0.1.0)

set(resource_file
    "st-helens/pre.data" 
	"st-helens/post.data"
    )


add_executable(surface-distance main.cpp)

foreach(file ${resource_file})
	configure_file(${file} ${PROJECT_BINARY_DIR} COPYONLY)
endforeach()


target_include_directories(surface-distance PRIVATE lib)
