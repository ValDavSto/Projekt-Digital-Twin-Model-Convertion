#ifndef GDS2STRUCTURE_H_
#define GDS2STRUCTURE_H_

#include <string>
#include <vector>
#include "Polygon.h"

class Gds2Structure {
private:
	std::string name;
	std::vector<Polygon> polygons;
	std::vector<Gds2Structure> structureReferences;
};

#endif
