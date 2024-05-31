#ifndef ICFACTORY_H_
#define ICFACTORY_H_


#include <vector>
#include <string>

#include "Polygon.h"
#include "Layer.h"
#include "IC.h"
#include "Gds2Import.h"
class ICFactory{
private:
	static std::vector<Layer> getLayers(std::vector<Polygon>& polygons);
public:
	static IC generateIC(std::string name, std::string filepath);
};

#endif

