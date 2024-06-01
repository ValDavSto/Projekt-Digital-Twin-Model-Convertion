#include "Gds2Structure.h"

Gds2Structure::Gds2Structure(std::string name, std::vector<Polygon> polygons) : name(name), polygons(polygons){}

std::string Gds2Structure::getName(){ return std::string(); }

std::vector<Polygon> Gds2Structure::getPolygons(){ return std::vector<Polygon>(); }

std::vector<StructRef> Gds2Structure::getStructRef(){ return std::vector<StructRef>(); }

void Gds2Structure::setStructRef(std::vector<StructRef> structRef) { structureReferences = structRef; }
