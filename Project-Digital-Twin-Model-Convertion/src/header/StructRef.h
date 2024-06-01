#ifndef STRUCTREF_H_
#define STRUCTREF_H_

#include <string>


// This class represents a gds2 strucutre reference. 
// It contains the name of the referenced strucutre and the coordinates where the "copy" should be palced
class StructRef{
private:
	std::string name;
	std::pair<int, int> coordinates;

public:
	StructRef(std::string name, std::pair<int, int> coordiantes);

	std::string getName();
	std::pair<int, int> getCoordinates();

	void setName(std::string name);
	void setCoordinates(std::pair<int, int> coordinates);
};

#endif

