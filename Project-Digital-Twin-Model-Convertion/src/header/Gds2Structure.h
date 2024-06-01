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
public:
	Gds2Structure(std:: string name, std::vector<Polygon> polygons);

	std::string getName();
	std::vector<Polygon> getPolygons();
	std::vector<Gds2Structure> getStructRef();

	void setStructRef(std::vector<Gds2Structure> structRef);
};


#endif
