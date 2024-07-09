#ifndef LAYER_H_
#define LAYER_H_

#include <vector>

#include "Polygon.h"
#include "Gds2Path.h"


class Layer {
private:
	int id; // number of the layer
	float thikness; // thikness of printed ploygons on this layer
	float height; // height of the layer
	std::vector<MyPolygon> polygons; // polygons which are located on the layer
	std::vector<Gds2Path> paths;

public:
	Layer(int id_, std::vector<MyPolygon> poly, float height_, float thickness_);

	int getId();
	float getThikness();
	float getHeight();
	std::vector<MyPolygon> getPolygons();
};

#endif