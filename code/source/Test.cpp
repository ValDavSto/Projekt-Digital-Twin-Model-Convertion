#include <vector>
#include <string>
#include <iostream>
#include "Polygon.h"
#include "Layer.h"
#include "IC.h"



int main() {
	std::vector<std::pair<float, float>> c1;

	c1.push_back(std::make_pair(2.5f, 3.2f));
	c1.push_back(std::make_pair(1.0f, 3.2f));
	c1.push_back(std::make_pair(2.4f, 7.2f));
	c1.push_back(std::make_pair(2.9f, 8.2f));

	unsigned int layer = 20;

	Polygon p1(layer, c1);
	Polygon p2(layer, c1);

	Polygon p4(10, c1);
	Polygon p3(10, c1);

	std::vector<Polygon> polygons;
	polygons.push_back(p1);
	polygons.push_back(p2);
	
	std::vector<Polygon> polygons2;
	polygons2.push_back(p4);
	polygons2.push_back(p3);

	


	std::cout << std::to_string(p1.getLayer()) << std::endl;

	Layer l1(layer, polygons, 2.3f, 2.4f);
	Layer l2(10, polygons2, 3.3f, 4.4f);

	std::vector<Layer> layers1;
	layers1.push_back(l1);
	layers1.push_back(l2);

	IC ic1("test", layers1);

	std::cout << ic1.getName() << std::endl;


	std::cout << std::to_string(l1.getThikness()) << std::endl;

}