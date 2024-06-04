#ifndef GDS2IMPORT_H_
#define GDS2IMPORT_H_

#include <vector>
#include <string>
#include "Polygon.h"
#include "StructRef.h"
#include "Gds2Structure.h"



class Gds2Import{
private:
	uint32_t readPosition;

	uint32_t getReadPosition();
	void setReadPosition(uint32_t currentPosition);
	int getWordInt(std::byte a, std::byte b);
	int getCoordinate(std::byte a, std::byte b, std::byte c, std::byte d);
	int getElemSize(std::byte a, std::byte b);
	std::string getStructName(int readPosition, std::vector<std::byte>& data);
	std::vector<std::pair<int, int>> getXY(std::vector<std::byte>& data);
	StructRef getStructRef(uint32_t filesize, std::vector<std::byte>& data);
	Polygon getPolygon(uint32_t filesize, std::vector<std::byte>& data);
public:
	Gds2Import();

	std::vector<Gds2Structure> getStructures(std::vector<std::byte> data);
	std::vector<Polygon> getPolygons(std::vector<std::byte> data);
	std::vector<std::byte> readFileData(const std::string& name);
	

};

#endif

