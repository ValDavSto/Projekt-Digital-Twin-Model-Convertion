#include "Gds2Structure.h"

Gds2Structure::Gds2Structure(std::string name, std::vector<Polygon> polygons) : name(name), polygons(polygons), structureReferences({}) {}

Gds2Structure::Gds2Structure(std::string name) : name(name), polygons({}), structureReferences({}) {}

std::string Gds2Structure::getName(){ return std::string(); }

std::vector<Polygon> Gds2Structure::getPolygons(){ return std::vector<Polygon>(); }

std::vector<StructRef> Gds2Structure::getStructRef(){ return std::vector<StructRef>(); }

void Gds2Structure::setStructRef(std::vector<StructRef> structRef) { structureReferences = structRef; }

void Gds2Structure::setPolygons(std::vector<Polygon> polygons) { this->polygons = polygons; }

void Gds2Structure::addStuctRef(StructRef newStrRef){
	structureReferences.push_back(newStrRef);
}
