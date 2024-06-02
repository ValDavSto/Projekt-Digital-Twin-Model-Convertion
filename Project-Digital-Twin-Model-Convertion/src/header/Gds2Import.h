#ifndef GDS2IMPORT_H_
#define GDS2IMPORT_H_

#include <vector>
#include <string>
#include "Polygon.h"



class Gds2Import{
private:
	int getWordInt(std::byte a, std::byte b);
	int getCoordinates(std::byte a, std::byte b, std::byte c, std::byte d);
public:
	std::vector<Polygon> getPolygons(std::vector<std::byte> data);
	std::vector<std::byte> readFileData(const std::string& name);

};

#endif

