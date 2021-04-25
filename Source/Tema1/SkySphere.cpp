#include "SkySphere.h"
#include <iostream>

#define rad(x) x*0.0174532925

glm::vec3 get_coords(float r, float az, float pol)
{
	az = rad(az);
	pol = rad(pol);
	return glm::vec3( r * sin(pol) * cos(az), r * cos(pol), r * sin(pol) * sin(az));
}

SkySphere::SkySphere()
{
}

SkySphere::SkySphere(std::string name, int points_polar, int points_azimuthal)
{
	int r = 1;
	float theta, phi, x, y, z;
	float inc_pol = 180 / points_polar;
	float inc_az = 360 / points_azimuthal;

	std::vector<VertexFormat> vertices;

	for(float i = 0; i <= 180; i+=inc_pol)
		for (float j = 0; j <= 360; j+=inc_az)
		{
			glm::vec3 coords = get_coords(1, j, i);
			vertices.push_back(VertexFormat(coords, glm::vec3(1.f), coords, glm::vec2(j / 360, i / 180)));
		}

	std::vector<unsigned short> indices;

	int base = 0;
	int i, j;
	for (i = 0; i < points_polar; i++)
	{
		for (j = 0; j <= points_azimuthal; j++)
		{
			indices.push_back(base + j); indices.push_back(base + j + 1); indices.push_back(base + j + points_azimuthal+1);
			indices.push_back(base + j + 1); indices.push_back(base + j + points_azimuthal+1); indices.push_back(base + j + points_azimuthal + 2);
		}
		base += points_azimuthal+1;
	}
	
	/*
	for (i = 0; i < vertices_height - 1; i++)
		for (j = 0; j < vertices_width - 1; j++)
		{
			int base = i * vertices_width + j;
			indices.push_back(base); indices.push_back(base + vertices_width);  indices.push_back(base + vertices_width + 1);
			indices.push_back(base); indices.push_back(base + vertices_width + 1); indices.push_back(base + 1);
		}
	*/

	mesh_sphere = new Mesh(name);
	mesh_sphere->InitFromData(vertices, indices);
}

Mesh* SkySphere::getMesh()
{
	return mesh_sphere;
}