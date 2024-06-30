#include "ICFactory.h"

#include <map>
#include <iostream>


std::vector<Layer> ICFactory::getLayers(std::vector<MyPolygon>& polygons, std::vector<std::pair<int, std::vector<int>>>& stackUpInfo) {
	std::map<int, std::vector<MyPolygon>> layerMap = {};

	// polygons get mapped to the corresponding layer
	for (auto& polygon : polygons) {
		layerMap[polygon.getLayer()].push_back(polygon);
	}

	// create layer objects
	// TODO: thikness and 
	std::vector<Layer> layers = {};

	if (stackUpInfo.size() != layerMap.size()) {
		for (auto& layer : layerMap) {

		std::cout << "Layer : " << layer.first << "; " << "Number of Polygons: " << layer.second.size() << std::endl;

		Layer newLayer(layer.first, layer.second, 2.0, 2.0);
		layers.push_back(newLayer);
		}
	}
	else {
		for (auto& pair : stackUpInfo) {
			Layer newLayer(pair.second[0], layerMap[pair.second[0]], pair.second[1], pair.second[2]);
			layers.push_back(newLayer);
		}
	}
	

	return layers;
}

IC ICFactory::generateIC(std::string name, std::string filepath, std::string stackUpPath) {
	Gds2Import import(filepath);
	std::vector<std::pair<int, std::vector<int>>> stackUpInfo = import.readStackUp(stackUpPath);
	std::vector<MyPolygon> polygons = import.getPolygons();
	std::vector<Layer> layers = getLayers(polygons, stackUpInfo);

	IC new_IC(name, layers);

	return new_IC;
}
