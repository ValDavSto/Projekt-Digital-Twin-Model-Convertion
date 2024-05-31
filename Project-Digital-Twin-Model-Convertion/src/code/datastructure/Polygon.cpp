#include "Polygon.h"

Polygon::Polygon(unsigned int l, std::vector<std::pair<int, int>> c) : layer(l), coordinates(c){}

unsigned int Polygon::getLayer() {
	return layer;
}

std::vector<std::pair<int, int>> Polygon::getCoordinates() {
	return coordinates;
}
