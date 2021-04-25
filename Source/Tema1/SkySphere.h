#pragma once
#include <include/glm.h>
#include <Core/GPU/Mesh.h>

class SkySphere
{
	public:
		SkySphere();
		SkySphere(std::string name, int points_polar, int points_azimuthal);
		Mesh* getMesh();
	private:
		Mesh* mesh_sphere;
};