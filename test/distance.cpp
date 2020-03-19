#include "catch.hpp"
#include "distance.h"


static glm::vec3 interpolatePixelAttribute(glm::ivec2 begin, glm::ivec2 end, 
	const std::vector<unsigned char> heights, int imageWidth, int imageHeight, float pixelDistance, float pixelHeight, float t) 
{
	int indFrom = begin.y * imageWidth + begin.x; 
	int indTo = end.y * imageWidth + end.x;
	float heightFrom = heights[indFrom] * pixelHeight;
	float heightTo = heights[indTo] * pixelHeight;

	glm::vec3 attribFrom = glm::vec3(pixelDistance * begin.x, pixelDistance * begin.y, heightFrom);
	glm::vec3 attribTo = glm::vec3(pixelDistance * end.x, pixelDistance * end.y, heightTo);
	return attribFrom + t * (attribTo - attribFrom);
}


static glm::vec3 interpolateIntersects(Ray ray, 
	glm::ivec2 begin, glm::ivec2 end, 
	const std::vector<unsigned char> heights, int imageWidth, int imageHeight, float pixelDistance, float pixelHeight) 
{
	auto intersect = intersectRayAndLine(ray, begin, end);
	return interpolatePixelAttribute(begin, end, heights, imageWidth, imageHeight, pixelDistance, pixelHeight, intersect.line);
}


TEST_CASE("Test ray intersect a line", "[distance]") {
	SECTION("Parallel") {
		auto rayBegin = glm::vec2(0.0f, 2.0f);
		auto rayEnd = glm::vec2(3.0f, 0.0f);
		auto lineBegin = glm::vec2(0.0f, 1.0f);
		auto lineEnd = glm::vec2(1.5, 0.0f);
		auto intersect = intersectRayAndLine(Ray{ rayBegin, glm::normalize(rayEnd - rayBegin) }, lineBegin, lineEnd);
		REQUIRE(intersect.type == IntersectionType::Parallel);
	}

	SECTION("Colinear") {
		auto rayBegin = glm::vec2(0.0f, 2.0f);
		auto rayEnd = glm::vec2(0.0f, 4.0f);
		auto lineBegin = glm::vec2(0.0f, 0.0f);
		auto lineEnd = glm::vec2(0.0f, 5.0f);
		auto intersect = intersectRayAndLine(Ray{ rayBegin, glm::normalize(rayEnd - rayBegin) }, lineBegin, lineEnd);
		REQUIRE(intersect.type == IntersectionType::Colinear);
	}

	SECTION("Intersect") {
		auto rayBegin = glm::vec2(1.0f, 5.0f);
		auto rayEnd = glm::vec2(5.0f, 3.0f);
		auto lineBegin = glm::vec2(2.0f, 3.0f);
		auto lineEnd = glm::vec2(4.0f, 5.0f);
		auto intersect = intersectRayAndLine(Ray{ rayBegin, glm::normalize(rayEnd - rayBegin) }, lineBegin, lineEnd);
		REQUIRE(intersect.type == IntersectionType::Intersect);
		REQUIRE(intersect.line == Approx(0.5));
		REQUIRE(intersect.ray == Approx(2.23606));
	}
}


TEST_CASE("Test voxels passed through by a diagonal line", "[distance]") {
	SECTION("Vertical line") {
		glm::ivec2 begin{ 1, 1 };
		glm::ivec2 end{ 1, 4 };
		auto voxels = traverseRayAndVoxels(begin, end, 4, 4);
		REQUIRE(voxels.size() == 3);
		REQUIRE(voxels[0] == glm::ivec2(1, 1));
		REQUIRE(voxels[1] == glm::ivec2(1, 2));
		REQUIRE(voxels[2] == glm::ivec2(1, 3));

		glm::ivec2 reversedBegin{ 1, 4 };
		glm::ivec2 reversedEnd{ 1, 1 };
		auto reversedVoxels = traverseRayAndVoxels(reversedBegin, reversedEnd, 6, 6);
		REQUIRE(reversedVoxels.size() == 3);
		REQUIRE(reversedVoxels[0] == glm::ivec2(1, 3));
		REQUIRE(reversedVoxels[1] == glm::ivec2(1, 2));
		REQUIRE(reversedVoxels[2] == glm::ivec2(1, 1));
	}

	SECTION("Horizontal line") {
		glm::ivec2 begin{ 1, 1 };
		glm::ivec2 end{ 4, 1 };
		auto voxels = traverseRayAndVoxels(begin, end, 4, 4);
		REQUIRE(voxels.size() == 3);
		REQUIRE(voxels[0] == glm::ivec2(1, 1));
		REQUIRE(voxels[1] == glm::ivec2(2, 1));
		REQUIRE(voxels[2] == glm::ivec2(3, 1));

		glm::ivec2 reversedBegin{ 4, 1 };
		glm::ivec2 reversedEnd{ 1, 1 };
		auto reversedVoxels = traverseRayAndVoxels(reversedBegin, reversedEnd, 6, 6);
		REQUIRE(reversedVoxels.size() == 3);
		REQUIRE(reversedVoxels[0] == glm::ivec2(3, 1));
		REQUIRE(reversedVoxels[1] == glm::ivec2(2, 1));
		REQUIRE(reversedVoxels[2] == glm::ivec2(1, 1));
	}

	SECTION("Nearly vertical line") {
		glm::ivec2 begin{ 1, 1 };
		glm::ivec2 end{ 2, 6 };
		auto voxels = traverseRayAndVoxels(begin, end, 6, 6);
		REQUIRE(voxels.size() == 5);
		REQUIRE(voxels[0] == glm::ivec2(1, 1));
		REQUIRE(voxels[1] == glm::ivec2(1, 2));
		REQUIRE(voxels[2] == glm::ivec2(1, 3));
		REQUIRE(voxels[3] == glm::ivec2(1, 4));
		REQUIRE(voxels[4] == glm::ivec2(1, 5));

		glm::ivec2 reversedBegin{ 2, 6 };
		glm::ivec2 reversedEnd{ 1, 1 };
		auto reversedVoxels = traverseRayAndVoxels(reversedBegin, reversedEnd, 6, 6);
		REQUIRE(reversedVoxels.size() == 5);
		REQUIRE(reversedVoxels[0] == glm::ivec2(1, 5));
		REQUIRE(reversedVoxels[1] == glm::ivec2(1, 4));
		REQUIRE(reversedVoxels[2] == glm::ivec2(1, 3));
		REQUIRE(reversedVoxels[3] == glm::ivec2(1, 2));
		REQUIRE(reversedVoxels[4] == glm::ivec2(1, 1));
	}

	SECTION("Nearly horizontal line") {
		glm::ivec2 begin{ 1, 1 };
		glm::ivec2 end{ 6, 2 };
		auto voxels = traverseRayAndVoxels(begin, end, 6, 6);
		REQUIRE(voxels.size() == 5);
		REQUIRE(voxels[0] == glm::ivec2(1, 1));
		REQUIRE(voxels[1] == glm::ivec2(2, 1));
		REQUIRE(voxels[2] == glm::ivec2(3, 1));
		REQUIRE(voxels[3] == glm::ivec2(4, 1));
		REQUIRE(voxels[4] == glm::ivec2(5, 1));

		glm::ivec2 reversedBegin{ 6, 2 };
		glm::ivec2 reversedEnd{ 1, 1 };
		auto reversedVoxels = traverseRayAndVoxels(reversedBegin, reversedEnd, 6, 6);
		REQUIRE(reversedVoxels.size() == 5);
		REQUIRE(reversedVoxels[0] == glm::ivec2(5, 1));
		REQUIRE(reversedVoxels[1] == glm::ivec2(4, 1));
		REQUIRE(reversedVoxels[2] == glm::ivec2(3, 1));
		REQUIRE(reversedVoxels[3] == glm::ivec2(2, 1));
		REQUIRE(reversedVoxels[4] == glm::ivec2(1, 1));
	}

	SECTION("Positive slope with abs(slope) > 1") {
		glm::ivec2 begin{ 1, 1 };
		glm::ivec2 end{ 4, 5 };
		auto voxels = traverseRayAndVoxels(begin, end, 6, 6);
		REQUIRE(voxels.size() == 6);
		REQUIRE(voxels[0] == glm::ivec2(1, 1));
		REQUIRE(voxels[1] == glm::ivec2(1, 2));
		REQUIRE(voxels[2] == glm::ivec2(2, 2));
		REQUIRE(voxels[3] == glm::ivec2(2, 3));
		REQUIRE(voxels[4] == glm::ivec2(3, 3));
		REQUIRE(voxels[5] == glm::ivec2(3, 4));

		glm::ivec2 reversedBegin{ 4, 5 };
		glm::ivec2 reversedEnd{ 1, 1 };
		auto reversedVoxels = traverseRayAndVoxels(reversedBegin, reversedEnd, 6, 6);
		REQUIRE(reversedVoxels.size() == 6);
		REQUIRE(reversedVoxels[0] == glm::ivec2(3, 4));
		REQUIRE(reversedVoxels[1] == glm::ivec2(3, 3));
		REQUIRE(reversedVoxels[2] == glm::ivec2(2, 3));
		REQUIRE(reversedVoxels[3] == glm::ivec2(2, 2));
		REQUIRE(reversedVoxels[4] == glm::ivec2(1, 2));
		REQUIRE(reversedVoxels[5] == glm::ivec2(1, 1));
	}

	SECTION("Positive slope with abs(slope) < 1") {
		glm::ivec2 begin{ 1, 2 };
		glm::ivec2 end{ 5, 4 };
		auto voxels = traverseRayAndVoxels(begin, end, 6, 6);
		REQUIRE(voxels.size() == 5);
		REQUIRE(voxels[0] == glm::ivec2(1, 2));
		REQUIRE(voxels[1] == glm::ivec2(2, 2));
		REQUIRE(voxels[2] == glm::ivec2(2, 3));
		REQUIRE(voxels[3] == glm::ivec2(3, 3));
		REQUIRE(voxels[4] == glm::ivec2(4, 3));

		glm::ivec2 reversedBegin{ 5, 4 };
		glm::ivec2 reversedEnd{ 1, 2 };
		auto reversedVoxels = traverseRayAndVoxels(reversedBegin, reversedEnd, 6, 6);
		REQUIRE(reversedVoxels.size() == 5);
		REQUIRE(reversedVoxels[0] == glm::ivec2(4, 3));
		REQUIRE(reversedVoxels[1] == glm::ivec2(3, 3));
		REQUIRE(reversedVoxels[2] == glm::ivec2(3, 2));
		REQUIRE(reversedVoxels[3] == glm::ivec2(2, 2));
		REQUIRE(reversedVoxels[4] == glm::ivec2(1, 2));
	}

	SECTION("Negative slope with abs(slope) > 1") {
		glm::ivec2 begin{ 1, 5 };
		glm::ivec2 end{ 3, 1 };
		auto voxels = traverseRayAndVoxels(begin, end, 6, 6);
		REQUIRE(voxels.size() == 5);
		REQUIRE(voxels[0] == glm::ivec2(1, 4));
		REQUIRE(voxels[1] == glm::ivec2(1, 3));
		REQUIRE(voxels[2] == glm::ivec2(1, 2));
		REQUIRE(voxels[3] == glm::ivec2(2, 2));
		REQUIRE(voxels[4] == glm::ivec2(2, 1));

		glm::ivec2 reversedBegin{ 3, 1 };
		glm::ivec2 reversedEnd{ 1, 5 };
		auto reversedVoxels = traverseRayAndVoxels(reversedBegin, reversedEnd, 6, 6);
		REQUIRE(reversedVoxels.size() == 5);
		REQUIRE(reversedVoxels[0] == glm::ivec2(2, 1));
		REQUIRE(reversedVoxels[1] == glm::ivec2(2, 2));
		REQUIRE(reversedVoxels[2] == glm::ivec2(2, 3));
		REQUIRE(reversedVoxels[3] == glm::ivec2(1, 3));
		REQUIRE(reversedVoxels[4] == glm::ivec2(1, 4));
	}

	SECTION("Negative slope with abs(slope) < 1") {
		glm::ivec2 begin{ 1, 5 };
		glm::ivec2 end{ 5, 3 };
		auto voxels = traverseRayAndVoxels(begin, end, 6, 6);
		REQUIRE(voxels.size() == 5);
		REQUIRE(voxels[0] == glm::ivec2(1, 4));
		REQUIRE(voxels[1] == glm::ivec2(2, 4));
		REQUIRE(voxels[2] == glm::ivec2(2, 3));
		REQUIRE(voxels[3] == glm::ivec2(3, 3));
		REQUIRE(voxels[4] == glm::ivec2(4, 3));

		glm::ivec2 reversedBegin{ 5, 3 };
		glm::ivec2 reversedEnd{ 1, 5 };
		auto reversedVoxels = traverseRayAndVoxels(reversedBegin, reversedEnd, 6, 6);
		REQUIRE(reversedVoxels.size() == 5);
		REQUIRE(reversedVoxels[0] == glm::ivec2(4, 3));
		REQUIRE(reversedVoxels[1] == glm::ivec2(3, 3));
		REQUIRE(reversedVoxels[2] == glm::ivec2(3, 4));
		REQUIRE(reversedVoxels[3] == glm::ivec2(2, 4));
		REQUIRE(reversedVoxels[4] == glm::ivec2(1, 4));
	}
}


TEST_CASE("Test surface distance between two points", "[distance]") {
	SECTION("Line crosses only one voxel") {
		std::vector<unsigned char> heights = {
			1, 1, 3, 1, 
			1, 2, 1, 1,
			1, 1, 1, 1,
			1, 1, 1, 1
		};

		glm::ivec2 begin{ 1, 0 };
		glm::ivec2 end(2, 1);
		float distance = calcSurfaceDistance(begin, end, heights, 4, 4, 1, 1);
		float expectDistance = glm::distance(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.5f, 0.5f, 2.5f)) + 
							   glm::distance(glm::vec3(1.5f, 0.5f, 2.5f), glm::vec3(2.0f, 1.0f, 1.0f));

		REQUIRE(distance == Approx(expectDistance));
	}

	SECTION("Line crosses mountain now") {
		std::vector<unsigned char> heights = {
			1, 5, 3, 1, 5,
			1, 2, 1, 2, 6,
			5, 1, 8, 1, 7,
			1, 8, 1, 9, 8,
			4, 4, 6, 7, 8
		};

		glm::ivec2 begin{ 1, 0 };
		glm::ivec2 end(3, 4);
		float distance = calcSurfaceDistance(begin, end, heights, 5, 5, 1, 1);

		// find expected distance
		const int imageWidth = 5;
		const int imageHeight = 5;
		const float pixelDistance = 1.0f;
		const float pixelHeight = 1.0f;

		Ray ray{ begin, end - begin };
		std::vector<glm::vec3> intersects;
		intersects.push_back(interpolateIntersects(ray, glm::ivec2(2, 0), glm::ivec2(1, 0), heights, imageWidth, imageHeight, pixelDistance, pixelHeight));
		intersects.push_back(interpolateIntersects(ray, glm::ivec2(2, 0), glm::ivec2(1, 1), heights, imageWidth, imageHeight, pixelDistance, pixelHeight));
		intersects.push_back(interpolateIntersects(ray, glm::ivec2(1, 1), glm::ivec2(2, 1), heights, imageWidth, imageHeight, pixelDistance, pixelHeight));
		intersects.push_back(interpolateIntersects(ray, glm::ivec2(2, 1), glm::ivec2(1, 2), heights, imageWidth, imageHeight, pixelDistance, pixelHeight));
		intersects.push_back(interpolateIntersects(ray, glm::ivec2(1, 2), glm::ivec2(2, 2), heights, imageWidth, imageHeight, pixelDistance, pixelHeight));
		intersects.push_back(interpolateIntersects(ray, glm::ivec2(3, 2), glm::ivec2(2, 3), heights, imageWidth, imageHeight, pixelDistance, pixelHeight));
		intersects.push_back(interpolateIntersects(ray, glm::ivec2(2, 3), glm::ivec2(3, 3), heights, imageWidth, imageHeight, pixelDistance, pixelHeight));
		intersects.push_back(interpolateIntersects(ray, glm::ivec2(3, 3), glm::ivec2(2, 4), heights, imageWidth, imageHeight, pixelDistance, pixelHeight));
		intersects.push_back(interpolateIntersects(ray, glm::ivec2(2, 4), glm::ivec2(3, 4), heights, imageWidth, imageHeight, pixelDistance, pixelHeight));
		
		float expectDistance = 0.0f;
		for (int i = 0; i < intersects.size() - 1; ++i) {
			expectDistance += glm::distance(intersects[i], intersects[i + 1]);
		}

		REQUIRE(distance == Approx(expectDistance));
	}
}
