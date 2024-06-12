#include "Layer.h"

Layer::Layer(int id_, std::vector<MyPolygon> poly, float thickness_, float height_)
	: id(id_), polygons(poly), thikness(thickness_), height(height_) {}

int Layer::getId() { return id; }

float Layer::getThikness() { return thikness; }

float Layer::getHeight() { return height; }

std::vector<MyPolygon> Layer::getPolygons() { return polygons; }