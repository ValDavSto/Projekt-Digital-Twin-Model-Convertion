#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <filesystem>
#include <fstream>

#include "Polygon.h"
#include "Layer.h"
#include "IC.h"
#include "Gds2Import.h"
#include "ICFactory.h"





void printBinary(std::vector<std::byte> data) {
	for (size_t i = 0; i < data.size(); ++i) {
		if (i % 16 == 0) {
			if (i != 0) std::cout << std::endl; // Neue Zeile zwischen den Zeilen
			std::cout << std::setfill('0') << std::setw(3) << std::hex << i << " "; // Index
		}
		if (i % 4 == 0) std::cout << " "; // Leerzeichen zwischen den Gruppen
		std::cout << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned int>(data[i]); // Daten
		if (i % 4 == 3 && i % 16 != 15) std::cout << " "; // Leerzeichen nach jeder 4er-Gruppe
	}
}




int main() {
	//std::string path = R"(C:\Users\Valentin\source\repos\occtest\testgds2.GDS)";
	std::string path = R"(C:\Users\Valentin\OneDrive\Info\Master\SoSe 24\Projekt Digitalization\Example Files\gdsii\nand2.gds)";
	std::vector<std::byte> filedata(Gds2Import::readFileData(path));
	IC new_IC = ICFactory::generateIC("Test", path);
	
	//std::vector<Polygon> polygons = Gds2Import::getPolygons(filedata);




	//printBinary(filedata);
	



	
	/*
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


	std::cout << std::to_string(l1.getThikness()) << std::endl;*/

}