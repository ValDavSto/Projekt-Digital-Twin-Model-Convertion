#include "StructRef.h"

StructRef::StructRef(std::string name, std::pair<int, int> coordiantes) : name(name), coordinates(coordinates){}

std::string StructRef::getName(){ return std::string(); }

std::pair<int, int> StructRef::getCoordinates(){ return std::pair<int, int>(); }

void StructRef::setName(std::string name) {	this->name = name; }

void StructRef::setCoordinates(std::pair<int, int> coordinates) { this->name = name; }
