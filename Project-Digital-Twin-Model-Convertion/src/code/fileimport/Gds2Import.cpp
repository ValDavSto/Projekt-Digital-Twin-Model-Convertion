#include "Gds2Import.h"
#include <filesystem>
#include <fstream>
#include <iostream>

// macros which define tags in a gds2 file
#define BGNSTR 0x0502 // befin of a structure
#define STRNAME 0x0606 // name of structure
#define BOUNDARY 0x0800 // begin of a boundary / polygon
#define SREF 0x0A00 // bein of an strucutre reference elemnet, a copy of the referenced strucuture will be placed at the given coordinates
#define SNAME 0x1206 // name of the referenced strucutre
#define LAYER 0x0D02 // begin of a layer
#define XY 0x1003  // list of coordinates, each coordinate is a 4 byte long singed integer


int Gds2Import::getWordInt(std::byte a, std::byte b) {
	return (int)a << 8 | (int)b;
}

int Gds2Import::getCoordinates(std::byte a, std::byte b, std::byte c, std::byte d) {
	return static_cast<int>((static_cast<uint32_t>(a) << 24) | (static_cast<uint32_t>(b) << 16) | static_cast<uint32_t>(c) << 8 | static_cast<uint32_t>(d));
}

std::vector<Polygon> Gds2Import::getPolygons(std::vector<std::byte> data) {
	unsigned int layer = 0;
	uint32_t filesize = data.size();
	std::vector<Polygon> polygons = {};
	for (int i = 0; i < filesize - 1; i++) {
		unsigned int boundarySize = 0;
		// i iterates through whole file to find boundarys
		if (getWordInt(data[i], data[i + 1]) == BOUNDARY) { // 0800 denotes the start of an boundary/polygon in a gdsii file
			int j = i;
			boundarySize = getWordInt(data[i - 2], data[i - 1]) / 2;
			bool isBoundary = false;


			for (j; j < filesize - 1; j++) {
				
				// j iterates through a boundary
				if (getWordInt(data[j], data[j + 1]) == LAYER) { // 0D02 denotes a layer 
					layer = (unsigned int)data[j + 3];
					std::cout << "Layer: " << (unsigned int)data[j + 3] << std::endl;
					isBoundary = true;
				}

				if (getWordInt(data[j], data[j + 1]) == XY && isBoundary) { // 1003 denotes the xy coordinates of the boundary/polygon
					
					unsigned int entrySize = getWordInt(data[j - 2], data[j - 1]) / 2; // the word befor the start of the xy coordinates denotes the number of coordinates
					std::cout << "entry size: " << entrySize << " bytes" << std::endl;

					std::vector<std::pair<int, int>> coordinates = {};

					int position = j + 2; // Position of the byte which is read from the filedata
					std::cout << "Position: " << position << std::endl;

					
					// extract coordinates of the boundary/polygon
					// each coordinate is 4 bytes long
					// iterates over the coordinates
					for (int k = 0; k < (entrySize - 2) / 4; k++) {
						//std::cout << "Position: " << position << std::endl;

						int x = getCoordinates(data[position], data[position + 1], data[position + 2], data[position + 3]);
						int y = getCoordinates(data[position + 4], data[position + 5], data[position + 6], data[position + 7]);

						std::cout << "xy: (" << x << ", " << y << ")" << std::endl;

						std::pair<int, int> xy = std::make_pair(x, y);
						coordinates.push_back(xy);
						position += 8;
					}
					isBoundary = false;

					Polygon newPolygon(layer, coordinates);
					polygons.push_back(newPolygon);
				}
			}
			// all important information from the boundary is retreved, the program continues to read after the boundary coordinates 
			// TODO: can be optimised if the size of the hole boundary element is stored, so it can skip to the end
			i = i + boundarySize;
		}
	}
	return polygons;
}
std::vector<std::byte> Gds2Import::readFileData(const std::string& name){
	std::filesystem::path inputFilePath{ name };
	auto length = std::filesystem::file_size(inputFilePath);
	std::cout << "Filelength[Byte]: " << length << std::endl;
	if (length == 0) {
		return {};  // empty vector
	}
	std::vector<std::byte> buffer(length);
	std::ifstream inputFile(name, std::ios_base::binary);
	inputFile.read(reinterpret_cast<char*>(buffer.data()), length);
	inputFile.close();
	return buffer;
}