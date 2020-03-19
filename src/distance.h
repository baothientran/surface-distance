#ifndef DISTANCE_H
#define DISTANCE_H

#include <vector>
#include "glm/glm.hpp"


enum class IntersectionType {
	Parallel,
	Colinear,
	Intersect
};


struct RayLineIntersection {
	IntersectionType type;
	float ray;
	float line;
};


struct Ray {
	glm::vec2 origin;
	glm::vec2 direction;
};


/*********
Find the intersection between a ray and a line.
The function will determine if the ray and line are parallel, colinear, or intersected.
If they are intersected, the function will return the intersection point such that:
intersection point = ray.origin + ray.direction * RayLineIntersection.ray;
intersection point = lineBegin + (lineEnd - lineBegin) * RayLineIntersection.line
**********/
RayLineIntersection intersectRayAndLine(Ray ray, glm::vec2 lineBegin, glm::vec2 lineEnd);


/*********
The function will return the list of voxels that a 2D line passes through in a 2D grid.
http://www.cse.yorku.ca/~amana/research/grid.pdf
**********/
std::vector<glm::ivec2> traverseRayAndVoxels(glm::ivec2 begin, glm::ivec2 end, int gridWidth, int gridHeight);


/*********
Find the surface distance between two points accounting for the topology of the surface.
Approach:
- First it finds the list of voxels the line passes through in a standard 2D grid (distance between two points is one) 
- For each voxel, it will find the intersection points between the line and voxel boundary (4 boundary lines making up the square and the diagonal line of the voxel as well)
- For each voxel and for each intersection points, linear interpolation will be used to determine the real coordinate of the intersection point 
in term of latitude, longtitude, and height in unit meter
- It then finds the distance between two consecutive intersection points along the line and add them up together.  

Drawback:
- The function does suffer floating point computation when grid is larger than 512x512 (I only tests grid 1512x1512, 4512x4512) 
**********/
float calcSurfaceDistance(glm::ivec2 begin, glm::ivec2 end, const std::vector<unsigned char>& heightdata, int imageWidth, int imageHeight, float pixelDistance, float pixelHeight);


#endif // !DISTANCE_H
