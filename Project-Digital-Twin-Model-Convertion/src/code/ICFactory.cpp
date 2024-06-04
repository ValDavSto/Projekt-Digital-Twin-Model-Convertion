#include "ICFactory.h"

#include <map>
#include <iostream>


std::vector<Layer> ICFactory::getLayers(std::vector<Polygon>& polygons) {
	std::map<int, std::vector<Polygon>> layerMap = {};

	// polygons get mapped to the corresponding layer
	for (auto& polygon : polygons) {
		layerMap[polygon.getLayer()].push_back(polygon);
	}

	// create layer objects
	// TODO: thikness and 
	std::vector<Layer> layers = {};

	for (auto& layer : layerMap) {

		std::cout << "Layer : " << layer.first << "; " << "Number of Polygons: " << layer.second.size() << std::endl;

		Layer newLayer(layer.first, layer.second, 2.0, 2.0);
		layers.push_back(newLayer);
	}

	return layers;
}

IC ICFactory::generateIC(std::string name, std::string filepath) {
	Gds2Import import(filepath);
	std::vector<Polygon> polygons = import.getPolygons();
	std::vector<Layer> layers = getLayers(polygons);

	IC new_IC(name, layers);

	return new_IC;
}
