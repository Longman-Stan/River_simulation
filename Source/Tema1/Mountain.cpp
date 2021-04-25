#include "Mountain.h"

Mountain::Mountain()
{

}

Mountain::Mountain(std::vector<glm::vec3> bezier_points, int start1, int start2, int num_points, std::string heightmap_path, int no_points_per_curve, float width1, float width2)
{
	int i,j;
	int idx1, idx2;
	int vert_ind;
	float t, tn;
	glm::vec3 p1, p2, p3, p4;
	std::vector<VertexFormat> vertices;
	std::vector<unsigned short> indices;

	for (i = 0; i < num_points; i += 3)
	{
		idx1 = start1 + i;
		idx2 = start2 + i;
		/*for (j = 0; j < no_points_per_curve; j++)
		{
			t = float(i) / no_points_per_curve;
			tn = float(i+1) / no_points_per_curve;
			p1 = bezier(&bezier_points[idx1], t);
			p1.x += width1;
			p2 = bezier(&bezier_points[idx1], t);
			p2.x += width1;
			p3 = bezier(&bezier_points[idx2], tn);
			p3.x -= width2;
			p4 = bezier(&bezier_points[idx2], tn);
			p4.x -= width2;
		}
		*/
	}

	mesh = new Mesh(std::string("mountaint") + std::to_string(start1));
	mesh->InitFromData(vertices, indices);
}

Mesh* get_mesh();