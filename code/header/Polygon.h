#ifndef POLYGON_H_
#define POLYGON_H_

#include <vector>




class Polygon{
private: 
		unsigned int layer;
		std::vector<std::pair<float, float>> coordinates;

public:
	Polygon(unsigned int l, std::vector<std::pair<float, float>> c);
	//void setCoordinates(vector<pair<float, float>>);
	//void setLayer(unsigned int );

	unsigned int getLayer();
	std::vector<std::pair<float, float>> getCoordinates();

};

#endif // !POLYGON_H_
