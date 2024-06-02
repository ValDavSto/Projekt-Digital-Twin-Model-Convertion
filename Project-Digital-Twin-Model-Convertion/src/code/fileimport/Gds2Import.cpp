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

// takes 2 std::byte objects, add them together and converts them into an int (unsigned)
int Gds2Import::getWordInt(std::byte a, std::byte b) {
	return (int)a << 8 | (int)b;
}

// gds2 coordinates are 4byte long, those 4 bytes are converted to a signed int
int Gds2Import::getCoordinates(std::byte a, std::byte b, std::byte c, std::byte d) {
	return static_cast<int>((static_cast<uint32_t>(a) << 24) | (static_cast<uint32_t>(b) << 16) | static_cast<uint32_t>(c) << 8 | static_cast<uint32_t>(d));
}

std::string Gds2Import::getStructureName(int readPosition, int size, std::vector<std::byte>& data){
	std::string structName(size, '\0');
	readPosition += 2; // the recived readposiont is at the first byte which denotes a STRNAME or SNAME (each 2 byte long). To read the string the program starts 2 bytes later at the first byte which represents a char
	size += size - 4; // size is reduced because it contains 2 bytes which contain the entry size and 2 bytes which are the STRNAME or SNAME tag

	int strIt = 0;

	for (readPosition; readPosition < size; readPosition++) {
		structName[strIt] = static_cast<char>(data[readPosition]);
		strIt++;
	}
	return structName;
}



void Gds2Import::getPol(std::vector<std::byte> data) {
	uint32_t filesize = data.size();

	for (int i = 0; i < filesize - 1; i++) {
		if (getWordInt(data[i], data[i + 1]) == STRNAME) {
			int elementSize = getWordInt(data[i - 2], data[i - 1]) / 2;
			int strIt = i + elementSize - 1; // StructureIterator skips unimportant information of the STRNAME element

			for (strIt; strIt < filesize - 1; strIt++) {

				if (getWordInt(data[strIt], data[strIt + 1]) == BOUNDARY) {

				}
				if (getWordInt(data[strIt], data[strIt + 1]) == SREF) {

				}
			}

		}
	}
}
std::vector<Polygon> Gds2Import::getPolygons(std::vector<std::byte> data) {
	unsigned int layer = 0;
	int filesize = data.size();
	std::vector<Polygon> polygons = {};
	bool isBoundary = false;


	for (int i = 0; i < filesize - 1; i++) {
		unsigned int boundarySize = 0;
		// i iterates through whole file to find boundarys
		if (getWordInt(data[i], data[i + 1]) == BOUNDARY) { // 0800 denotes the start of an boundary/polygon in a gdsii file
			int j = i;
			boundarySize = getWordInt(data[i - 2], data[i - 1]) / 2;
			isBoundary = true;

			while (isBoundary && j < filesize -1) {
				// j iterates through a boundary
				if (getWordInt(data[j], data[j + 1]) == LAYER) { // 0D02 denotes a layer 
					layer = (unsigned int)data[j + 3];
					std::cout << "Layer: " << (unsigned int)data[j + 3] << std::endl;
				}

				if (getWordInt(data[j], data[j + 1]) == XY) { // 1003 denotes the xy coordinates of the boundary/polygon

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

				j++;
			}
			i = j;
			
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