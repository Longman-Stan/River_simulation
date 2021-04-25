#pragma once
#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include <string>

class Boat
{
	Mesh* mesh;
public:
	Boat();
	Boat(std::string name);
	Mesh* get_mesh();
};