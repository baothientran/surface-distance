- How to build the project:
+ create the build directory in the project directory: mkdir build
+ go to build: cd build
+ run cmake: cmake ..
+ run make command for linux: make


- executable should be in build/src/surface_distance_exe:
+ ./surface_distance_exe [begin_pixel_X] [begin_pixel_Y] [end_pixel_X] [end_pixel_Y]


- test should be in build/test/test_surface_distance:
+ ./test_surface_distance


- Approach to find the surface distance between two points accounting for the topology of the surface.
+ First it finds the list of voxels the line passes through in a standard 2D grid (distance between two points is one) 
+ For each voxel, it will find the intersection points between the line and voxel boundary (4 boundary lines making up the square and the diagonal line of the voxel as well)
+ For each voxel and for each intersection points, linear interpolation will be used to determine the real coordinate of the intersection point 
in term of latitude, longtitude, and height in unit meter
+ It then finds the distance between two consecutive intersection points along the line and add them up together.  

Drawback:
+ The function does suffer floating point computation when grid is larger than 512x512 (I only tests grid 1512x1512, 4512x4512) 

