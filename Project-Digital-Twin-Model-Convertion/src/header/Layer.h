#ifndef LAYER_H_
#define LAYER_H_

#include <vector>

#include "Polygon.h"


class Layer{
private:
	int id; // number of the layer
	float thikness; // thikness of printed ploygons on this layer
	float height; // height of the layer
	std::vector<Polygon> polygons; // polygons which are located on the layer

public:
	Layer(int id_, std::vector<Polygon> poly, float thickness_, float height_);

	int getId();
	float getThikness();
	float getHeight();
	std::vector<Polygon> getPolygons();
};

#endif

