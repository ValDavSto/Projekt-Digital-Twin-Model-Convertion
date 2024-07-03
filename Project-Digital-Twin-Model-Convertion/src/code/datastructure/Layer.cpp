#include "Layer.h"

Layer::Layer(int id_, std::vector<MyPolygon> poly, float height_, float thickness_)
	: id(id_), polygons(poly), height(height_), thikness(thickness_) {}

int Layer::getId() { return id; }

float Layer::getThikness() { return thikness; }

float Layer::getHeight() { return height; }

std::vector<MyPolygon> Layer::getPolygons() { return polygons; }