#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include <array>
#include "glm/glm.hpp"


const float PIXEL_DISTANCE = 30.0f;
const float PIXEL_HEIGHT = 11.0f;
const int IMG_WIDTH = 512;
const int IMG_HEIGHT = 512;
const int GRID_WIDTH = IMG_WIDTH - 1;
const int GRID_HEIGHT = IMG_HEIGHT - 1;


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


float cross(glm::vec2 a, glm::vec2 b) {
	return a.x * b.y - a.y * b.x;
}


int sub2ind(int width, int x, int y) {
	return y * width + x;
}


glm::ivec2 toVoxelCoord(glm::ivec2 coord, glm::vec2 direction, int gridWidth, int gridHeight) {
	int voxelX = coord.x;
	int voxelY = coord.y;

	int stepX = direction.x < 0 ? -1 : 1;
	int stepY = direction.y < 0 ? -1 : 1;
	if (stepX == 1 && stepY == -1) {
		--voxelY;
	}
	else if (stepX == -1 && stepY == -1) {
		--voxelX;
		--voxelY;
	}
	else if (stepX == -1 && stepY == 1) {
		--voxelX;
	}

	return glm::ivec2{ voxelX, voxelY };
}


RayLineIntersection intersectRayAndLine(Ray ray, glm::vec2 lineBegin, glm::vec2 lineEnd) {
	glm::vec2 beginOrigin = lineBegin - ray.origin;
	glm::vec2 endBegin = lineEnd - lineBegin;
	float rayLineCross = cross(ray.direction, endBegin);
	float beginOriginRayCross = cross(beginOrigin, ray.direction);

	// line and ray are colinear
	if (glm::abs(rayLineCross) < std::numeric_limits<float>::epsilon() && 
		glm::abs(beginOriginRayCross) < std::numeric_limits<float>::epsilon()) 
	{
		return RayLineIntersection{ IntersectionType::Colinear, -1.0f, -1.0f };
	}

	// line and ray are parallel 
	if (glm::abs(rayLineCross) < std::numeric_limits<float>::epsilon()) {
		return RayLineIntersection{ IntersectionType::Parallel, -1.0f, -1.0f };
	}

	RayLineIntersection intersect{ IntersectionType::Intersect, -1.0f, -1.0f};
	intersect.ray = cross(beginOrigin, endBegin) / rayLineCross;
	intersect.line = cross(beginOrigin, ray.direction) / rayLineCross;

	return intersect;
}


float intersectRayAndLowerUpperVoxelBound(Ray ray, glm::vec2 beginLower, glm::vec2 endLower, glm::vec2 beginUpper, glm::vec2 endUpper) {
	RayLineIntersection lowerIntersect = intersectRayAndLine(ray, beginLower, endLower);
	RayLineIntersection upperIntersect = intersectRayAndLine(ray, beginUpper, endUpper);

	if (lowerIntersect.type == IntersectionType::Intersect && upperIntersect.type == IntersectionType::Intersect) {
		return std::max(lowerIntersect.ray, upperIntersect.ray);
	}

	return std::numeric_limits<float>::max();
}


std::vector<glm::ivec2> traverseRayAndVoxels(glm::ivec2 begin, glm::ivec2 end, int gridWidth, int gridHeight) {
	Ray ray{begin, glm::normalize(static_cast<glm::vec2>(end-begin))};

	// find the begin voxel 
	glm::ivec2 voxel = toVoxelCoord(begin, ray.direction, gridWidth, gridHeight);
	int voxelX = voxel.x;
	int voxelY = voxel.y;

	// find stepX and stepY depends on the direction of ray
	int stepX = ray.direction.x < 0 ? -1 : 1;
	int stepY = ray.direction.y < 0 ? -1 : 1;

	// find tMaxX, tMaxY
	float tMaxX = intersectRayAndLowerUpperVoxelBound(
		ray, 
		voxel, voxel + glm::ivec2(0, 1),
		voxel + glm::ivec2(1, 0), voxel + glm::ivec2(1, 1));

	float tMaxY = intersectRayAndLowerUpperVoxelBound(
		ray,
		voxel, voxel + glm::ivec2(1, 0),
		voxel + glm::ivec2(0, 1), voxel + glm::ivec2(1, 1));

	// find tDeltaX, tDeltaY
	float tDeltaX = glm::abs(1.0f / ray.direction.x);
	float tDeltaY = glm::abs(1.0f / ray.direction.y);

	glm::ivec2 endVoxel = toVoxelCoord(end, -ray.direction, gridWidth, gridHeight);
	std::vector<glm::ivec2> voxels;
	while ((voxelX >= 0 && voxelX < gridWidth) && (voxelY >= 0 && voxelY < gridHeight) &&  // out of bound condition
			(voxelX != endVoxel.x || voxelY != endVoxel.y))								   // We arrive at the destination voxel
	{
		voxels.push_back(glm::ivec2(voxelX, voxelY));
		if (tMaxX < tMaxY) {
			tMaxX += tDeltaX;
			voxelX = voxelX + stepX;
		}
		else {
			tMaxY += tDeltaY;
			voxelY = voxelY + stepY;
		}
	}

	if (voxelX == endVoxel.x && voxelY == endVoxel.y) {
		voxels.push_back(glm::ivec2(voxelX, voxelY));
	}

	return voxels;
}


void intersectLineAndVoxelBounds(glm::ivec2 begin, glm::ivec2 end, const std::vector<glm::ivec2> voxelBounds, 
	std::function<void(glm::ivec2, glm::ivec2, float, float)> intersectCallback,
	std::function<void(glm::ivec2, glm::ivec2, float, float)> colinearCallback) 
{
	Ray ray{ begin, static_cast<glm::vec2>(end - begin) };
	for (std::size_t i = 0; i < voxelBounds.size() - 1; ++i) {
		RayLineIntersection intersect = intersectRayAndLine(ray, voxelBounds[i], voxelBounds[i+1]);

		if (intersect.type == IntersectionType::Colinear) {
			glm::vec2 voxelBoundVector = static_cast<glm::vec2>(voxelBounds[i + 1] - voxelBounds[i]);
			float voxelBoundLength = glm::dot(voxelBoundVector, voxelBoundVector);
			float tline0 = glm::dot(static_cast<glm::vec2>(begin - voxelBounds[i]), voxelBoundVector) / voxelBoundLength;
			float tline1 = tline0 + glm::dot(static_cast<glm::vec2>(end-begin), voxelBoundVector) / voxelBoundLength;

			// if two lines are overlapped
			if (tline1 >= 0 && tline0 <= 1) {
				tline0 = std::max(tline0, 0.0f);
				tline1 = std::min(tline1, 1.0f);
				colinearCallback(voxelBounds[i], voxelBounds[i+1], tline0, tline1);
			}
		}
		else if (intersect.type == IntersectionType::Intersect && intersect.line >= 0.0f && intersect.line <= 1.0f) {
			intersectCallback(voxelBounds[i], voxelBounds[i+1], intersect.line, intersect.ray);
		}
	}
}


float calcSurfaceDistance(glm::ivec2 begin, glm::ivec2 end, const std::vector<unsigned char> &heightdata) {
	auto voxels = traverseRayAndVoxels(begin, end, GRID_WIDTH, GRID_HEIGHT);
	float distance = 0.0f;
	for (glm::ivec2 voxel : voxels) {
		std::vector<glm::ivec2> voxelBound = {
			glm::ivec2(voxel.x + 1, voxel.y), glm::ivec2(voxel.x, voxel.y),
			glm::ivec2(voxel.x, voxel.y + 1), glm::ivec2(voxel.x + 1, voxel.y + 1),
			glm::ivec2(voxel.x + 1, voxel.y), glm::ivec2(voxel.x, voxel.y + 1)
		};

		bool colinear = false;
		float colinearDistance = 0.0f;
		std::vector<std::pair<float, glm::vec3>> intersects;
		intersects.reserve(5);
		intersectLineAndVoxelBounds(begin, end, voxelBound,
			[&](glm::ivec2 from, glm::ivec2 to, float tline, float tray) {
				int indFrom = sub2ind(IMG_WIDTH, from.x, from.y);
				int indTo = sub2ind(IMG_WIDTH, to.x, to.y);
				float heightFrom = heightdata[indFrom] * PIXEL_HEIGHT;
				float heightTo = heightdata[indTo] * PIXEL_HEIGHT;

				glm::vec3 attribFrom = glm::vec3(PIXEL_DISTANCE * from.x, PIXEL_DISTANCE * from.y, heightFrom);
				glm::vec3 attribTo = glm::vec3(PIXEL_DISTANCE * to.x, PIXEL_DISTANCE * to.y, heightTo);
				glm::vec3 attribIntersect = attribFrom + tline * (attribTo - attribFrom);
				intersects.push_back(std::make_pair(tray, attribIntersect));
			},
			[&](glm::ivec2 from, glm::ivec2 to, float tline0, float tline1) {
				colinear = true;
				int indFrom = sub2ind(IMG_WIDTH, from.x, from.y);
				int indTo = sub2ind(IMG_WIDTH, to.x, to.y);
				float heightFrom = heightdata[indFrom] * PIXEL_HEIGHT;
				float heightTo = heightdata[indTo] * PIXEL_HEIGHT;

				glm::vec3 attribFrom = glm::vec3(PIXEL_DISTANCE * from.x, PIXEL_DISTANCE * from.y, heightFrom);
				glm::vec3 attribTo = glm::vec3(PIXEL_DISTANCE * to.x, PIXEL_DISTANCE * to.y, heightTo);
				glm::vec3 p0 = attribFrom + tline0 * (attribTo - attribFrom);
				glm::vec3 p1 = attribFrom + tline1 * (attribTo - attribFrom);
				colinearDistance = glm::distance(p0, p1);
			});

		if (colinear) {
			distance += colinearDistance;
		}
		else if (!intersects.empty()) {
			std::sort(intersects.begin(), intersects.end(), 
				[](const auto& a, const auto& b) { return a.first < b.first; });

			for (int i = 0; i < intersects.size() - 1; ++i) {
				float currDistance = glm::distance(intersects[i].second, intersects[i + 1].second);
				distance += currDistance;
			}
		}
	}

	return distance;
}


std::vector<unsigned char> readHeightData(const std::string &filename) {
	std::ifstream file(filename, std::ios::binary);
	file.unsetf(std::ios::skipws);

    std::streampos fileSize;
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

	std::vector<unsigned char> height;
	height.reserve(fileSize);
	height.insert(height.begin(), std::istream_iterator<unsigned char>(file), std::istream_iterator<unsigned char>());

	return height;
}


int main(int argv, char** args) {
	std::vector<unsigned char> preHeight = readHeightData("pre.data");
	std::vector<unsigned char> postHeight = readHeightData("post.data");

	glm::ivec2 begin{1, 1};
	glm::ivec2 end{ 512, 512 };
	float preDistance = calcSurfaceDistance(begin, end, preHeight);
	float postDistance = calcSurfaceDistance(begin, end, postHeight);

	std::cout << "Pre Distance: " << preDistance << "\n";
	std::cout << "Post Distance: " << postDistance << "\n";

	return 0;
}

