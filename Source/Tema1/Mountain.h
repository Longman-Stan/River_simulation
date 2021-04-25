#pragma once
#include <vector>
#include <string>
#include <include/glm.h>
#include <Core/GPU/Mesh.h>

class Mountain
{
	Mesh* mesh;
public:
	Mountain();
	Mountain(std::vector<glm::vec3> bezier_points, int start1, int start2, int num_points, std::string heightmap_path, int no_points_per_curve, float width1, float width2);
	Mesh* get_mesh();
};