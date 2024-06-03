#ifndef GDS2IMPORT_H_
#define GDS2IMPORT_H_

#include <vector>
#include <string>
#include "Polygon.h"



class Gds2Import{
private:
	int getWordInt(std::byte a, std::byte b);
	int getCoordinates(std::byte a, std::byte b, std::byte c, std::byte d);
	int getElemSize(std::byte a, std::byte b);
	std::string getStructName(int readPosition, int size, std::vector<std::byte>& data);
	std::pair<Polygon, int> getPolygon(int readPosition, uint32_t filesize, std::vector<std::byte>& data);
public:
	void getPol(std::vector<std::byte> data);
	std::vector<Polygon> getPolygons(std::vector<std::byte> data);
	std::vector<std::byte> readFileData(const std::string& name);
	

};

#endif

