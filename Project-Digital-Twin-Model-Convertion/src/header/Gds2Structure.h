#ifndef GDS2STRUCTURE_H_
#define GDS2STRUCTURE_H_

#include <string>
#include <vector>
#include "Polygon.h"
#include "StructRef.h"


// This class represents a gds2 structure whith all its polygons and structure references
// WARNING: only the boundary type is tracked
class Gds2Structure {
private:
	std::string name;
	std::vector<Polygon> polygons;
	std::vector<StructRef> structureReferences;
public:
	Gds2Structure(std::string name, std::vector<Polygon> polygons);
	Gds2Structure(std::string name);

	std::string getName();
	std::vector<Polygon> getPolygons();
	std::vector<StructRef> getStructRef();

	void setStructRef(std::vector<StructRef> structRef);
	void setPolygons(std::vector<Polygon> polygons);
};


#endif
