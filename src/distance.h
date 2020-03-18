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


RayLineIntersection intersectRayAndLine(Ray ray, glm::vec2 lineBegin, glm::vec2 lineEnd);


std::vector<glm::ivec2> traverseRayAndVoxels(glm::ivec2 begin, glm::ivec2 end, int gridWidth, int gridHeight);


float calcSurfaceDistance(glm::ivec2 begin, glm::ivec2 end, const std::vector<unsigned char>& heightdata, int imageWidth, int imageHeight, float pixelDistance, float pixelHeight);


#endif // !DISTANCE_H
