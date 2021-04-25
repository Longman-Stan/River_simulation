#include "Boat.h"

std::vector<unsigned short> get_indices()
{
	std::vector<unsigned short> indices = {
		//up
		0,1,2,
		0,2,6,
		6,2,5,
		5,2,3,
		5,3,4,
		//bot
		7,13,12,
		7,12,11,
		13,8,9,
		13,9,12,
		11,12,10,
		12,9,10,
		//left
		11,7,0,
		11,0,6,
		//right
		8,9,2,
		8,2,1,
		//back
		13,8,7,
		7,8,1,
		7,1,0,
		//front
		9,3,2,
		9,10,3,
		3,10,4,
		10,5,4,
		10,11,5,
		5,11,6
	};
	return indices;
}

std::vector<VertexFormat> get_vertices()
{
	std::vector<VertexFormat> vertices = {
		//up
		VertexFormat( glm::vec3(1, 1, -2), glm::vec3(1), glm::vec3(1,1,0)), //0
		VertexFormat(glm::vec3(-1, 1, -2), glm::vec3(1), glm::vec3(-1,1,0)), //1
		VertexFormat(glm::vec3(-1, 1, 2), glm::vec3(1), glm::vec3(-1,1,0)), //2
		VertexFormat(glm::vec3(-0.8, 1, 3), glm::vec3(1), glm::vec3(-0.8,1,1)), //3
		VertexFormat(glm::vec3(0, 1, 4), glm::vec3(1), glm::vec3(0,1,1)), //4
		VertexFormat(glm::vec3(0.8, 1, 3), glm::vec3(1), glm::vec3(0.8,1,1)), //5
		VertexFormat(glm::vec3(1, 1, 2), glm::vec3(1), glm::vec3(1,1,0)), //6
		//down
		VertexFormat(glm::vec3(1, 0, -2), glm::vec3(1), glm::vec3(-1,0,-1)), //7
		VertexFormat(glm::vec3(-1, 0, -2), glm::vec3(1), glm::vec3(1,0,-1)), //8
		VertexFormat(glm::vec3(-1, 0, 2), glm::vec3(1), glm::vec3(1,0,0)), //9
		VertexFormat(glm::vec3(0, 0, 3), glm::vec3(1), glm::vec3(0,-1,1)), //10
		VertexFormat(glm::vec3(1, 0, 2), glm::vec3(1), glm::vec3(-1,0,0)), //11
		VertexFormat(glm::vec3(0, -1, 2), glm::vec3(1), glm::vec3(0,-1,0)), //12
		VertexFormat(glm::vec3(0, -1, -2), glm::vec3(1), glm::vec3(0,-1,0)), //13
	};
	return vertices;
}

Boat::Boat()
{
	mesh = new Mesh("default");
	mesh->InitFromData(get_vertices(), get_indices());
}

Boat::Boat(std::string name)
{
	mesh = new Mesh(name);
	mesh->InitFromData(get_vertices(), get_indices());
}

Mesh* Boat::get_mesh()
{
	return mesh;
}