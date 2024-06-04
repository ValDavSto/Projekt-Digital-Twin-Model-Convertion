#include "Gds2Import.h"
#include <filesystem>
#include <fstream>
#include <iostream>

// macros which define tags in a gds2 file
#define BGNSTR 0x0502 // befin of a structure
#define STRNAME 0x0606 // name of a structure
#define ENDSTR 0x0700 // end of a structure
#define BOUNDARY 0x0800 // begin of a boundary / polygon
#define SREF 0x0A00 // bein of an strucutre reference elemnet, a copy of the referenced strucuture will be placed at the given coordinates
#define SNAME 0x1206 // name of the referenced strucutre
#define LAYER 0x0D02 // begin of a layer
#define XY 0x1003  // list of coordinates, each coordinate is a 4 byte long singed integer

Gds2Import::Gds2Import() : readPosition(0){}


uint32_t Gds2Import::getReadPosition(){ return readPosition; }

void Gds2Import::setReadPosition(uint32_t currentPosition) { readPosition = currentPosition;  }

// takes 2 std::byte objects, add them together and converts them into an int (unsigned)
int Gds2Import::getWordInt(std::byte a, std::byte b) {
	return (int)a << 8 | (int)b;
}

// gds2 coordinates are 4byte long, those 4 bytes are converted to a signed int
int Gds2Import::getCoordinate(std::byte a, std::byte b, std::byte c, std::byte d) {
	return static_cast<int>((static_cast<uint32_t>(a) << 24) | (static_cast<uint32_t>(b) << 16) | static_cast<uint32_t>(c) << 8 | static_cast<uint32_t>(d));
}

int Gds2Import::getElemSize(std::byte a, std::byte b) {
	return getWordInt(a, b) / 2;
}

std::string Gds2Import::getStructName(int readPosition, std::vector<std::byte>& data){
	int size = getWordInt(data[readPosition - 2], data[readPosition - 1]);
	std::string structName(size, '\0');
	readPosition += 2; // the recived readposiont is at the first byte which denotes a STRNAME or SNAME (each 2 byte long). To read the string the program starts 2 bytes later at the first byte which represents a char
	size -= 4; // size is reduced because it contains 2 bytes which contain the entry size and 2 bytes which are the STRNAME or SNAME tag

	int strIt = 0;

	for (strIt; strIt < size; strIt++) {
		structName[strIt] = static_cast<char>(data[readPosition]);
		readPosition++;
	}
	return structName;
}

std::vector<std::pair<int, int>> Gds2Import::getXY(std::vector<std::byte>& data) {
	unsigned int entrySize = getWordInt(data[readPosition - 2], data[readPosition - 1]) / 2; // the word befor the start of the xy coordinates denotes the number of coordinates
	int coordIt = readPosition + 2; // Position of the byte which is read from the filedata
	std::vector<std::pair<int, int>> coordinates = {};
	
	//Print information aboout coordinates
	//std::cout << "entry size: " << entrySize << " bytes" << std::endl;
	//std::cout << "Position: " << coordIt << std::endl;

	// extract coordinates of the boundary/polygon
	// each coordinate is 4 bytes long
	// iterates over the coordinates
	for (int i = 0; i < (entrySize - 2) / 4; i++) {
		//std::cout << "Position: " << position << std::endl;

		int x = getCoordinate(data[coordIt], data[coordIt + 1], data[coordIt + 2], data[coordIt + 3]);
		int y = getCoordinate(data[coordIt + 4], data[coordIt + 5], data[coordIt + 6], data[coordIt + 7]);

		std::cout << "xy: (" << x << ", " << y << ")" << std::endl;

		std::pair<int, int> xy = std::make_pair(x, y);
		coordinates.push_back(xy);
		coordIt += 8;
	}

	return coordinates;
}

StructRef Gds2Import::getStructRef( uint32_t filesize, std::vector<std::byte>& data) {
	bool isStructRef = true;
	std::string strRefName = "";
	std::pair<int, int> coordinates = std::make_pair(0, 0);
	int i = readPosition;

	while (isStructRef && readPosition < filesize) {
		if (getWordInt(data[i], data[i + 1]) == SNAME) {
			strRefName = getStructName(i, data);
			std::cout << "Structure Reference: " << strRefName << std::endl;
		}
		if(getWordInt(data[i], data[i + 1]) == XY) {
			this->setReadPosition(i);
			coordinates = getXY(data)[0];
			isStructRef = false;
		}
		i++;
	}

	this->setReadPosition(i);
	return StructRef(strRefName, coordinates);

}

Polygon Gds2Import::getPolygon(uint32_t filesize, std::vector<std::byte>& data) {
	unsigned int layer = 0;
	bool isBoundary = true;
	bool containsLayer = false;
	Polygon poly = Polygon();
	int i = readPosition;

	while (isBoundary && i < filesize - 1) {
		// i iterates through a boundary
		if (getWordInt(data[i], data[i + 1]) == LAYER) { // 0D02 denotes a layer 
			containsLayer = true;
			layer = (unsigned int)data[i + 3];
			std::cout << "Layer: " << (unsigned int)data[i + 3] << std::endl;
		}

		if (getWordInt(data[i], data[i + 1]) == XY && containsLayer) { // 1003 denotes the xy coordinates of the boundary/polygon

			this->setReadPosition(i);
			std::vector<std::pair<int, int>> coordinates = getXY(data);
			
			isBoundary = false;
			containsLayer = false;

			poly.setCoordinates(coordinates);
			poly.setLayer(layer);
		}

		i++;
	}

	
	this->setReadPosition(i);

	return poly;
}



std::vector<Gds2Structure> Gds2Import::getStructures(std::vector<std::byte> data) {
	uint32_t filesize = data.size();
	std::vector<Polygon> polygons = {};
	bool isStruct = false;
	std::vector<Gds2Structure> structures = {};

	for (int i = 0; i < filesize - 1; i++) {
		if (getWordInt(data[i], data[i + 1]) == STRNAME) {
			isStruct = true;
			int elementSize = getWordInt(data[i - 2], data[i - 1]);
			std::string structName = getStructName(i, data);
			int strIt = i + elementSize - 1; // StructureIterator skips unimportant information of the STRNAME element
			Gds2Structure newStruct(structName);
			std::vector<Polygon> structPolys = {};

			std::cout << "Structure: " << structName << std::endl;

			while (isStruct && strIt < filesize - 1) {

				if ((getWordInt(data[strIt], data[strIt + 1]) != ENDSTR)) {
					if (getWordInt(data[strIt], data[strIt + 1]) == BOUNDARY) {
						this->setReadPosition(strIt);
						Polygon newPol = getPolygon(filesize, data);

						structPolys.push_back(newPol);
						strIt = this->getReadPosition();

					} else if (getWordInt(data[strIt], data[strIt + 1]) == SREF) {
						this->setReadPosition(strIt);
						StructRef newStrRef = getStructRef(filesize, data);
						newStruct.addStuctRef(newStrRef);
						strIt = this->getReadPosition();
					}
				}
				else {
					isStruct = false;
					i = strIt;
				}

				
				strIt++;
			}
			structures.push_back(newStruct);

		}
	}
	return structures;
}
std::vector<Polygon> Gds2Import::getPolygons(std::vector<std::byte> data) {
	unsigned int layer = 0;
	int filesize = data.size();
	std::vector<Polygon> polygons = {};
	bool isBoundary = false;


	for (int i = 0; i < filesize - 1; i++) {
		if (getWordInt(data[i], data[i + 1]) == STRNAME) {
			std::string structName = getStructName(i, data);
			std::cout << "Struct: " << structName << std::endl;
		}

		// i iterates through whole file to find boundarys
		/*if (getWordInt(data[i], data[i + 1]) == BOUNDARY) { // 0800 denotes the start of an boundary/polygon in a gdsii file
			std::cout << "Position Boundary start: " << i << std::endl;

			std::pair<Polygon, int> newPol = getPolygon(i, filesize, data);
			
			polygons.push_back(newPol.first);
			i = newPol.second;
			
		}*/
	}
	return polygons;
}
std::vector<std::byte> Gds2Import::readFileData(const std::string& name){
	std::filesystem::path inputFilePath{ name };
	uint32_t length = std::filesystem::file_size(inputFilePath);
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