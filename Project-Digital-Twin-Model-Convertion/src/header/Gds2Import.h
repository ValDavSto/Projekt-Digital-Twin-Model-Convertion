#ifndef GDS2IMPORT_H_
#define GDS2IMPORT_H_

#include <vector>
#include <string>

#include "Polygon.h"

class Gds2Import{
private:
	static int getWordInt(std::byte a, std::byte b);
	static int getCoordinates(std::byte a, std::byte b, std::byte c, std::byte d);
public:
	static std::vector<Polygon> getPolygons(std::vector<std::byte> data);
	static std::vector<std::byte> readFileData(const std::string& name);

};

#endif

