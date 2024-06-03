#include "Polygon.h"

Polygon::Polygon(unsigned int l, std::vector<std::pair<int, int>> c) : layer(l), coordinates(c){}

Polygon::Polygon() {
	layer = 0;
	coordinates = {};
}

void Polygon::setCoordinates(std::vector<std::pair<int, int>> coords){ coordinates = coords; }

void Polygon::setLayer(unsigned int layer) { this->layer = layer; }

unsigned int Polygon::getLayer() { return layer; }

std::vector<std::pair<int, int>> Polygon::getCoordinates() { return coordinates; }
