#version 430
layout(lines) in;
in int instance[2];
//TODO 

uniform mat4 View;
uniform mat4 Projection;
uniform vec3 control_p1[4];
uniform vec3 control_p2[4];
uniform int start_point;
uniform int end_point;
uniform int no_of_generated_points;
uniform int show_support_points;
uniform float width1, width2;
uniform int num_segment;

layout(location = 0) out vec2 texture_coord;
layout(location = 1) out vec3 world_position;
layout(location = 2) out vec3 world_normal;
//prima data generati o curba (cu line strip), apoi o suprafata de rotatie/translatie cu triangle_strip
layout(triangle_strip, max_vertices = 70) out;

vec3 get_height(vec3 point, int idx)
{
	float newY = idx * idx;
	return vec3(point.x, newY, point.z);
}

vec3 rotateY(vec3 point, float u)
{
	float x = point.x * cos(u) - point.z *sin(u);
	float z = point.x * sin(u) + point.z *cos(u);
	return vec3(x, point.y, z);
}

vec3 translateX(vec3 point, float t)
{
	return vec3(point.x + t, point.y, point.z);
}

vec3 bezier(vec3 cp[4], float t)
{
	return cp[0] * pow((1 - t), 3) + 
			cp[1] * 3 * t * pow((1 - t), 2) +
			cp[2] * 3 * pow(t, 2) * (1 - t) + 
			cp[3] * pow(t, 3);
}

void send_illumination_params(vec3 p, vec3 normal)
{
	world_position = p;
	world_normal = normal;
}

#define NUM_P 3

void make_mountain_segment(vec3 p1, vec3 p2, vec3 p3, vec3 p4, float t, float tn, int j)
{

	texture_coord = vec2(float(j)/NUM_P,t);
	send_illumination_params(p1, vec3(-1,0,0));
	gl_Position = Projection* View * vec4( p1, 1);	EmitVertex();

	texture_coord = vec2(float(j)/NUM_P,tn);
	send_illumination_params(p2, vec3(-1,0,0));
	gl_Position = Projection* View * vec4( p2, 1);	EmitVertex();

	texture_coord = vec2(float(j+1)/NUM_P,t);
	send_illumination_params(p3, vec3(1,0,0));
	gl_Position = Projection* View * vec4( p3, 1);	EmitVertex();

	texture_coord = vec2(float(j+1)/NUM_P,tn);
	send_illumination_params(p4, vec3(1,0,0));
	gl_Position = Projection* View * vec4( p4, 1);	EmitVertex();
	
	EndPrimitive();

}

void main()
{
	float t, tn;
	vec3 point1, point2, point3, point4, point5, point6;
	
	//TODO 
	//in loc sa emiteti varfuri reprezentand punctele de control, emiteti varfuri care sa aproximeze curba Bezier

	for(int i=start_point; i< end_point;i++)
	{
		t = float(i) / no_of_generated_points;
		tn = float(i+1) / no_of_generated_points;
		point1 = bezier(control_p1, t); 
		point1 = translateX(point1, width1 * width2);
		point1 = get_height(point1, 0);
		point2 = bezier(control_p1, tn);
		point2 = translateX(point2, width1 * width2);
		point2 = get_height(point2, 0);

		for(int j = 0; j < NUM_P; j ++)
		{
			point3 = translateX(point1, 5 * width2);
			point4 = translateX(point2, 5 * width2);
			point3 = get_height(point3, j);
			point4 = get_height(point4, j);
			make_mountain_segment(point1, point2, point3, point4, t, tn, j);
			point1 = point3;
			point2 = point4;
		}
		
	}



	if( show_support_points == 1)
	{
		gl_Position = Projection* View * vec4(control_p1[0], 1);	EmitVertex();
		gl_Position = Projection* View * vec4(control_p1[1], 1);	EmitVertex();
		gl_Position = Projection* View * vec4(control_p1[2], 1);	EmitVertex();
		gl_Position = Projection* View * vec4(control_p1[3], 1);	EmitVertex();
	}
	
	EndPrimitive();
}
