#include "Gds2Import.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cassert>

// macros which define tags in a gds2 file
#define BGNSTR 0x0502 // begin of a structure
#define STRNAME 0x0606 // name of a structure
#define ENDSTR 0x0700 // end of a structure
#define BOUNDARY 0x0800 // begin of a boundary / polygon
#define SREF 0x0A00 // begin of an structure reference element, a copy of the referenced structure will be placed at the given coordinates
#define SNAME 0x1206 // name of the referenced structure
#define LAYER 0x0D02 // begin of a layer
#define PATH 0x0900 // begin of path element
#define PATHTYPE 0x2102 // type of the path element, 3 different path types ( type 0 is default type if none is specified)
#define WIDTH 0x0f03 // width of the path
#define XY 0x1003  // list of coordinates, each coordinate is a 4 byte long singed integer

Gds2Import::Gds2Import() : readPosition(0) {}

Gds2Import::Gds2Import(std::string filePath) {
	data = readFileData(filePath);
	filesize = data.size();
	readPosition = 0;
}

inline void Gds2Import::checkSize(int x) {
	assert(x < filesize);
}


uint32_t Gds2Import::getReadPosition() { return readPosition; }

void Gds2Import::setReadPosition(uint32_t currentPosition) { readPosition = currentPosition; }

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

std::string Gds2Import::getStructName(int readPosition) {
	int size = getWordInt(data[readPosition - 2], data[readPosition - 1]);
	std::string structName(size, '\0');
	readPosition += 2; // the received read position is at the first byte which denotes a STRNAME or SNAME (each 2 byte long). To read the string the program starts 2 bytes later at the first byte which represents a char
	size -= 4; // size is reduced because it contains 2 bytes which contain the entry size and 2 bytes which are the STRNAME or SNAME tag

	int strIt = 0;

	for (strIt; strIt < size; strIt++) {
		structName[strIt] = static_cast<char>(data[readPosition]);
		readPosition++;
	}
	return structName;
}

std::vector<std::pair<int, int>> Gds2Import::getXY() {
	unsigned int entrySize = getWordInt(data[readPosition - 2], data[readPosition - 1]) - 4; // the word before the start of the xy coordinates denotes the number of coordinates; 4 is subtracted because 2 bytes are occupied by the XY-tag and 2 by the entrysize
	int coordIt = readPosition + 2; // Position of the byte which is read from the filedata
	std::vector<std::pair<int, int>> coordinates = {};

	//Print information about coordinates
	//std::cout << "entry size: " << entrySize << " bytes" << std::endl;
	//std::cout << "Position: " << coordIt << std::endl;

	// extract coordinates of the boundary/polygon
	// iterates over the coordinates
	// entrySize is devided by 4, because each coordinate is 4 byte long and than by 2, because the coordinates are extracted in pairs
	for (int i = 0; i < (entrySize / 4) / 2; i++) {
		//std::cout << "Position: " << position << std::endl;

		int x = getCoordinate(data[coordIt], data[coordIt + 1], data[coordIt + 2], data[coordIt + 3]);
		int y = getCoordinate(data[coordIt + 4], data[coordIt + 5], data[coordIt + 6], data[coordIt + 7]);

		//std::cout << "xy: (" << x << ", " << y << ")" << std::endl;

		std::pair<int, int> xy = std::make_pair(x, y);
		coordinates.push_back(xy);
		coordIt += 8;
	}

	return coordinates;
}

StructRef Gds2Import::getStructRef() {
	bool isStructRef = true;
	std::string strRefName = "";
	std::pair<int, int> coordinates = std::make_pair(0, 0);
	int i = readPosition;

	while (isStructRef && readPosition < filesize) {
		if (getWordInt(data[i], data[i + 1]) == SNAME) {
			strRefName = getStructName(i);
			std::cout << "Structure Reference: " << strRefName << std::endl;
		}
		if (getWordInt(data[i], data[i + 1]) == XY) {
			this->setReadPosition(i);
			coordinates = getXY()[0]; // strRef only contain one set of coordinates at which a copy of the referenced structure is placed
			isStructRef = false;
		}
		i++;
	}

	StructRef strRef = StructRef(strRefName, coordinates);
	std::cout << strRef.getCoordinates().first << " " << strRef.getCoordinates().second << std::endl;


	this->setReadPosition(i);
	return StructRef(strRefName, coordinates);

}

MyPolygon Gds2Import::getPolygon() {
	unsigned int layer = 0;
	bool isBoundary = true;
	bool containsLayer = false;
	MyPolygon poly = MyPolygon();
	int i = readPosition;

	while (isBoundary && i < filesize - 1) {
		// i iterates through a boundary
		if (getWordInt(data[i], data[i + 1]) == LAYER) { // 0D02 denotes a layer 
			containsLayer = true;
			layer = (unsigned int)data[i + 3];
			//std::cout << "Layer: " << (unsigned int)data[i + 3] << std::endl;
		}

		if (getWordInt(data[i], data[i + 1]) == XY && containsLayer) { // 1003 denotes the xy coordinates of the boundary/polygon

			this->setReadPosition(i);
			std::vector<std::pair<int, int>> coordinates = getXY();

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

Gds2Path Gds2Import::getPath() {
	bool isPath = true;
	bool containsLayer = false;
	Gds2Path newPath = Gds2Path();
	int i = readPosition;

	while (isPath && i < filesize - 1) {
		// i iterates through a boundary
		if (getWordInt(data[i], data[i + 1]) == LAYER) { // 0D02 denotes a layer 
			containsLayer = true;
			newPath.setLayer((unsigned int)data[i + 3]);
			//std::cout << "Layer: " << (unsigned int)data[i + 3] << std::endl;
		}

		if (getWordInt(data[i], data[i + 1]) == PATHTYPE && containsLayer) {
			newPath.setPathType((unsigned int)data[i + 3]);
		}

		if (getWordInt(data[i], data[i + 1]) == WIDTH && containsLayer) {
			newPath.setWidth(getCoordinate(data[i + 2], data[i + 3], data[i + 4], data[i + 5]));
		}

		if (getWordInt(data[i], data[i + 1]) == XY && containsLayer) { // 1003 denotes the xy coordinates of the boundary/polygon

			this->setReadPosition(i);
			newPath.setCoordinates(getXY());

			isPath = false;
			containsLayer = false;
		}

		i++;
	}


	this->setReadPosition(i);

	return newPath;
}




std::vector<Gds2Structure> Gds2Import::getStructures() {

	std::vector<MyPolygon> polygons = {};
	bool isStruct = false;
	std::vector<Gds2Structure> structures = {};
	std::vector<StructRef> structureReferences = {};
	std::vector<Gds2Path> paths = {};

	for (int i = 0; i < filesize - 2; i++) {

		checkSize(i + 1); // assertion which checks if the filedata has i+1 bytes
		if (getWordInt(data[i], data[i + 1]) == STRNAME) {
			isStruct = true;
			int elementSize = getWordInt(data[i - 2], data[i - 1]);
			std::string structName = getStructName(i);
			int strIt = i + elementSize - 1; // StructureIterator skips unimportant information of the STRNAME element
			Gds2Structure newStruct(structName); // create empty structure
			std::vector<MyPolygon> structPolys = {};

			std::cout << "Structure: " << structName << std::endl;

			while (isStruct && strIt < filesize - 1) {

				checkSize(i + 1); // assertion which checks if the filedata has i+1 bytes

				if ((getWordInt(data[strIt], data[strIt + 1]) != ENDSTR)) {

					checkSize(i + 1); // assertion which checks if the filedata has i+1 bytes

					if (getWordInt(data[strIt], data[strIt + 1]) == BOUNDARY) {
						this->setReadPosition(strIt); // set current read position for the gePolygon method
						MyPolygon newPol = getPolygon();

						structPolys.push_back(newPol);
						strIt = this->getReadPosition(); // is needed because the read position changed during the getPolygon method

					}
					else if (getWordInt(data[strIt], data[strIt + 1]) == SREF) {
						//same principle as getting a polygon see above
						this->setReadPosition(strIt);
						StructRef newStrRef = getStructRef();
						newStruct.addStuctRef(newStrRef);
						strIt = this->getReadPosition();
					}
					else if (getWordInt(data[strIt], data[strIt + 1]) == PATH) {
						this->setReadPosition(strIt);
						Gds2Path newPath = getPath();
						paths.push_back(newPath);
						strIt = this->getReadPosition();
					}
				}
				else { // end of structure
					isStruct = false;
					i = strIt;
				}


				strIt++;
			}
			newStruct.setPaths(paths);
			newStruct.setPolygons(structPolys);
			structures.push_back(newStruct);

		}
		// In the case that a boundary or strRef is not a part of struct there will be added in this section
		// !!! TODO currently they will be not returned by this method, this has to be added


		if (getWordInt(data[i], data[i + 1]) == BOUNDARY && getWordInt(data[i + 2], data[i + 3]) == LAYER) {
			this->setReadPosition(i);
			MyPolygon newPol = getPolygon();

			polygons.push_back(newPol);
			i = this->getReadPosition();
			std::cout << "Boundary added!" << std::endl;
		}

		if (getWordInt(data[i], data[i + 1]) == SREF) {
			this->setReadPosition(i);
			StructRef newStrRef = getStructRef();
			structureReferences.push_back(newStrRef);
			i = this->getReadPosition();
			std::cout << "SREF added!" << std::endl;
		}

	}
	// print information of the structures contained in the file
	for (auto& str : structures) {
		std::cout << str.getName() << " Polygons: " << str.getPolygons().size() << " StrRef: " << str.getStructRef().size() << std::endl;
	}
	return structures;
}

std::vector<MyPolygon> Gds2Import::getStrRefPolygons(Gds2Structure& structure, std::map<std::string, Gds2Structure>& structMap, std::vector<MyPolygon>& polygons) {


	for (auto& strRef : structure.getStructRef()) {
		std::pair<int, int> placement = strRef.getCoordinates();
		Gds2Structure referencedStruct = structMap[strRef.getName()];
		std::vector<MyPolygon> refPoly = referencedStruct.getPolygons();

		if (referencedStruct.getStructRef().size() > 0) {
			getStrRefPolygons(referencedStruct, structMap, polygons);
			// TODO Hier noch zukünftiges return value verarbeiten
		}
		for (auto& polygon : refPoly) {
			std::vector<std::pair<int, int>> coords = polygon.getCoordinates();
			std::vector<std::pair<int, int>> newCoords = {};

			for (auto& xy : coords) {
				xy = std::make_pair(xy.first + placement.first, xy.second + placement.second);
				newCoords.push_back(xy);
			}
			polygon.setCoordinates(newCoords);
			polygons.push_back(polygon);

		}
	}

	return polygons;
}


std::vector<MyPolygon> Gds2Import::getPolygons() {
	std::vector<Gds2Structure> structures = getStructures();
	std::vector<MyPolygon> polygons = {};
	std::map<std::string, Gds2Structure> structMap = {};

	for (auto& structure : structures) {
		structMap[structure.getName()] = structure;
	}

	for (auto& structure : structures) {
		std::vector<MyPolygon> structPolys = structure.getPolygons();
		std::vector<MyPolygon> emptyVec = {};

		if (structure.getStructRef().size() > 0) {
			std::vector<MyPolygon> strRefPolys = getStrRefPolygons(structure, structMap, emptyVec);
			structPolys.insert(structPolys.end(), strRefPolys.begin(), strRefPolys.end());
		}


		if (structure.getPaths().size() > 0) {
			std::vector<MyPolygon> paths = {};

			for (auto& path : structure.getPaths()) {
				paths.push_back(path.pathToPolygon());
			}

			structPolys.insert(structPolys.end(), paths.begin(), paths.end());
		}


		std::cout << "Structure: " << structure.getName() << " Polygons: " << structPolys.size() << std::endl;

		polygons.insert(polygons.end(), structPolys.begin(), structPolys.end());
		/*std::vector<MyPolygon> structPolys = structure.getPolygons();
		polygons.insert(polygons.end(), structPolys.begin(), structPolys.end());

		for (auto& strRef : structure.getStructRef()) {
			std::pair<int, int> placement = strRef.getCoordinates();
			Gds2Structure referencedStruct = structMap[strRef.getName()];
			std::vector<MyPolygon> refPoly = referencedStruct.getPolygons();

			for (auto& polygon : refPoly) {
				std::vector<std::pair<int, int>> coords = polygon.getCoordinates();
				std::vector<std::pair<int, int>> newCoords = {};

				for (auto& xy : coords) {
					xy = std::make_pair(xy.first + placement.first, xy.second + placement.second);
					newCoords.push_back(xy);
				}
				polygon.setCoordinates(newCoords);
				polygons.push_back(polygon);

			}
		}*/
	}

	return polygons;
}

std::vector<std::byte> Gds2Import::readFileData(const std::string& name) {
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
