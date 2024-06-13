#ifndef GDS2IMPORT_H_
#define GDS2IMPORT_H_

#include <vector>
#include <string>
#include <map>
#include "Polygon.h"
#include "StructRef.h"
#include "Gds2Structure.h"



class Gds2Import{
private:
	uint32_t readPosition;
	std::vector<std::byte> data;
	uint32_t filesize;

	inline void checkSize(int x);
	uint32_t getReadPosition();
	void setReadPosition(uint32_t currentPosition);
	int getWordInt(std::byte a, std::byte b);
	int getCoordinate(std::byte a, std::byte b, std::byte c, std::byte d);
	int getElemSize(std::byte a, std::byte b);
	std::string getStructName(int readPosition);
	std::vector<std::pair<int, int>> getXY();
	std::vector<MyPolygon> getStrRefPolygons(Gds2Structure str, std::map<std::string, Gds2Structure>& structMap, std::vector<MyPolygon>& polygons);
	StructRef getStructRef();
	MyPolygon getPolygon();
public:
	Gds2Import();
	Gds2Import(std::string filePath);

	std::vector<Gds2Structure> getStructures();
	std::vector<std::byte> readFileData(const std::string& name);
	std::vector<MyPolygon> getPolygons();
	

};

#endif

