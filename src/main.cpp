#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
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


void displayUsage() {
	std::cout << "Usage: [begin_pixel_X] [begin_pixel_Y] [end_pixel_X] [end_pixel_Y]" << "\n";
	std::cout << "begin_pixel_X: x component of the begin pixel. x >= 0 && x < 511\n";
	std::cout << "begin_pixel_Y: y component of the begin pixel. y >= 0 && y < 511\n";
	std::cout << "end_pixel_X: x component of the end pixel. x >= 0 && x < 511\n";
	std::cout << "end_pixel_Y: y component of the end pixel. y >= 0 && y < 511\n";
}


bool isCMDArgsInvalid(int component) {
	return component < 0 || component >= IMG_WIDTH;
}


int main(int argv, char** args) {
	if (argv != 5) {
		displayUsage();
		return 0;
	}

	int beginX;
	int beginY;
	int endX;
	int endY;
	try {
		beginX = std::stoi(args[1]);
		beginY = std::stoi(args[2]);
		endX   = std::stoi(args[3]);
		endY   = std::stoi(args[4]);
		if (isCMDArgsInvalid(beginX) || isCMDArgsInvalid(beginY) || isCMDArgsInvalid(endX) || isCMDArgsInvalid(endY)) {
			displayUsage();
			return 0;
		}
	}
	catch (const std::exception &e) {
		displayUsage();
		return 0;
	}

	std::vector<unsigned char> preHeight = readHeightData("pre.data");
	std::vector<unsigned char> postHeight = readHeightData("post.data");

	glm::ivec2 begin{beginX, beginY};
	glm::ivec2 end{ endX, endY };
	float preDistance = calcSurfaceDistance(begin, end, preHeight, IMG_WIDTH, IMG_HEIGHT, PIXEL_DISTANCE, PIXEL_HEIGHT);
	float postDistance = calcSurfaceDistance(begin, end, postHeight, IMG_WIDTH, IMG_HEIGHT, PIXEL_DISTANCE, PIXEL_HEIGHT);

	std::cout << "Pre Distance: " << preDistance << "\n";
	std::cout << "Post Distance: " << postDistance << "\n";
	std::cout << "difference: " << postDistance - preDistance << "\n";

	return 0;
}

