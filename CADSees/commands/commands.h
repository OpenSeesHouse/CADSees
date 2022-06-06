#include "../stdafx.h"
AcDbBlockTableRecord* getModelSpace(OpenMode mode); 
void setModel(std::vector<std::string> line);
void addElement(std::vector<std::string> line);
void addNode(std::vector<std::string> line);
void addRecorder(std::vector<std::string> line);
void ReadResponse(std::string folder);
double SetDeformedState(int n, double fac);
void addPile(std::vector<std::string> line);
void addCube(std::vector<std::string> line);