#ifndef GDS2IMPORT_H_
#define GDS2IMPORT_H_

#include <vector>
#include <string>
#include "Polygon.h"


// macros which define tags in a gds2 file
#define BGNSTR 0x0502 // befin of a structure
#define STRNAME 0x0606 // name of structure
#define BOUNDARY 0x0800 // begin of a boundary / polygon
#define SREF 0x0A00 // bein of an strucutre reference elemnet, a copy of the referenced strucuture will be placed at the given coordinates
#define SNAME 0x1206 // name of the referenced strucutre
#define LAYER 0x0D02 // begin of a layer
#define XY 0x1003  // list of coordinates, each coordinate is a 4 byte long singed integer


class Gds2Import{
private:
	int getWordInt(std::byte a, std::byte b);
	int getCoordinates(std::byte a, std::byte b, std::byte c, std::byte d);
public:
	std::vector<Polygon> getPolygons(std::vector<std::byte> data);
	std::vector<std::byte> readFileData(const std::string& name);

};

#endif

