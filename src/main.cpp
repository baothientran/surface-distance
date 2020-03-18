#include <iostream>
#include <fstream>
#include <iterator>
#include "distance.h"


const float PIXEL_DISTANCE = 30.0f;
const float PIXEL_HEIGHT = 11.0f;
const int IMG_WIDTH = 512;
const int IMG_HEIGHT = 512;


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
	glm::ivec2 end{ 512, 4 };
	float preDistance = calcSurfaceDistance(begin, end, preHeight, IMG_WIDTH, IMG_HEIGHT, PIXEL_DISTANCE, PIXEL_HEIGHT);
	float postDistance = calcSurfaceDistance(begin, end, postHeight, IMG_WIDTH, IMG_HEIGHT, PIXEL_DISTANCE, PIXEL_HEIGHT);

	std::cout << "Pre Distance: " << preDistance << "\n";
	std::cout << "Post Distance: " << postDistance << "\n";
	std::cout << "Straight Distance: " << glm::length(static_cast<glm::vec2>(end - begin)* PIXEL_DISTANCE) << "\n";

	return 0;
}

