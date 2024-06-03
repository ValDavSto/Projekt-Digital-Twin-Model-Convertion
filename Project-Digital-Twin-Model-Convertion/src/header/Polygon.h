#ifndef POLYGON_H_
#define POLYGON_H_

#include <vector>




class Polygon{
private: 
		unsigned int layer;
		std::vector<std::pair<int, int>> coordinates;

public:
	Polygon(unsigned int l, std::vector<std::pair<int, int>> c);
	Polygon();
	void setCoordinates(std::vector<std::pair<int, int>> coords);
	void setLayer(unsigned int layer);

	unsigned int getLayer();
	std::vector<std::pair<int, int>> getCoordinates();

};

#endif // !POLYGON_H_
