#include "Polygon.h"

Polygon::Polygon(unsigned int l, std::vector<std::pair<float, float>> c) : layer(l), coordinates(c){}

unsigned int Polygon::getLayer() {
	return layer;
}

std::vector<std::pair<float, float>> Polygon::getCoordinates() {
	return coordinates;
}
