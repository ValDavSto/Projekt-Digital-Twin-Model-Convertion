#ifndef GDS2PATH_H_
#define GDS2PATH_H_

#include <vector>
#include <string>
#include "Layer.h"

class Gds2Path {
private:
	int width; // width of the path
	unsigned int layer; 
	unsigned int pathType;
	std::vector<std::pair<int, int>> coordinates;


public:
	void setWidth(int width);
	void setLayer(unsigned int layer);
	void setPathType(unsigned int pathType);
	void setCoordinates(std::vector<std::pair<int, int>> coordinates);

};

#endif

