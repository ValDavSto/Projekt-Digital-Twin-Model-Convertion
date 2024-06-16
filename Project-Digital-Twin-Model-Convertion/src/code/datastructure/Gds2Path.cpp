#include "Gds2Path.h"

void Gds2Path::setWidth(int width) { this->width = width; }

void Gds2Path::setLayer(unsigned int layer) { this->layer = layer; }

void Gds2Path::setPathType(unsigned int pathType) { this->pathType = pathType; }

void Gds2Path::setCoordinates(std::vector<std::pair<int, int>> coordinates){ this->coordinates = coordinates; }
