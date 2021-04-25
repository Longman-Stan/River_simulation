#version 430
layout(lines) in;
//TODO 

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 control_p[4];
uniform int start_point;
uniform int end_point;
uniform int no_of_generated_points;
uniform int show_support_points;
uniform float width;
uniform float rotation;
uniform float time;

layout(location = 0) out vec2 texture_coord;
layout(location = 1) out vec3 world_position;
layout(location = 2) out vec3 world_normal;
layout(location = 3) out mat3 TBN;
//prima data generati o curba (cu line strip), apoi o suprafata de rotatie/translatie cu triangle_strip
layout(triangle_strip, max_vertices = 48) out;

in int instance[2];

int num_sines;
float L[5];
float W[5];
float A[5];
float S[5];
vec3 D[5];
float PHI[10];

#define K 3

vec3 get_height(vec3 point)
{
	float newy = 0;
	for(int i = 0; i< num_sines; i++)
		newy += 2 * A[i] * pow( sin( W[i] * dot(D[i], point) + time * PHI[i])/2, K) , 3; 
//	np.y = newy;
	return vec3(point.x, newy, point.z);
}

float get_dx(vec3 point)
{
	float res = 0;
	for(int i = 0; i< num_sines; i++)
		res += A[i] * W[i] * D[i].x * cos( W[i] * dot(D[i], point) + time * PHI[i])* pow( sin( W[i] * dot(D[i], point) + time * PHI[i])/2, K-1); 
	return res;
}

float get_dy(vec3 point)
{
	float res = 0;
	for(int i = 0; i< num_sines; i++)
		res += A[i] * W[i] * D[i].z * cos( W[i] * dot(D[i], point) + time * PHI[i]) * pow( sin( W[i] * dot(D[i], point) + time * PHI[i])/2, K-1); 
	return res;
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

vec3 bezier(int start_idx, float t)
{
	return control_p[start_idx] * pow((1 - t), 3) + 
			control_p[start_idx+1] * 3 * t * pow((1 - t), 2) +
			control_p[start_idx+2] * 3 * pow(t, 2) * (1 - t) + 
			control_p[start_idx+3] * pow(t, 3);
}

void send_illumination_params(vec3 p)
{
	float dx = get_dx(p);
	float dy = get_dy(p);
	vec3 B = vec3(1,dx,0);
	vec3 T = vec3(0,dy,1);
	vec3 N = vec3(-dx, 1, -dy);
	TBN = mat3(T,B,N);
	world_position = p;
	world_normal = N;
}

void make_river_segment(vec3 p1, vec3 p2, vec3 p3, vec3 p4, vec3 p5, vec3 p6, float t, float tn)
{
	texture_coord = vec2(0, t);
	send_illumination_params(p5);
	gl_Position = Projection* View * vec4( p5, 1);	EmitVertex();

	texture_coord = vec2(0, tn);
	send_illumination_params(p6);
	gl_Position = Projection* View * vec4( p6, 1);	EmitVertex();

	texture_coord = vec2(0.5, t);
	send_illumination_params(p1);
	gl_Position = Projection* View * vec4( p1, 1);	EmitVertex();

	texture_coord = vec2(0.5, tn);
	send_illumination_params(p2);
	gl_Position = Projection* View * vec4( p2, 1);	EmitVertex();

	texture_coord = vec2(0.5, t);
	send_illumination_params(p1);
	gl_Position = Projection* View * vec4( p1, 1);	EmitVertex();

	texture_coord = vec2(0.5, tn);
	send_illumination_params(p2);
	gl_Position = Projection* View * vec4( p2, 1);	EmitVertex();

	texture_coord = vec2(1, t);
	send_illumination_params(p3);
	gl_Position = Projection* View * vec4( p3, 1);	EmitVertex();

	texture_coord = vec2(1, tn);
	send_illumination_params(p4);
	gl_Position = Projection* View * vec4( p4, 1);	EmitVertex();
	EndPrimitive();
}

void main()
{
	float t, tn;
	vec3 point1, point2, point3, point4, point5, point6;

	L[0] = 0.12; A[0] = 0.05; S[0] = 0.15; D[0] = vec3(1,0,0);
	L[1] = 0.37; A[1] = 0.05; S[1] = 0.05; D[1] = vec3(0,0,1);
	L[2] = 0.12; A[2] = 0.06; S[2] = 0.1; D[2] = vec3(0.3,0,0.2);
	L[3] = 0.13; A[3] = 0.08; S[3] = 0.075; D[3] = vec3(0.8,0,-0.2);
	L[4] = 0.18; A[4] = 0.075; S[4] = 0.05; D[4] = vec3(0.6,0,-0.5);
	num_sines = 5;
	for(int i = 0; i < num_sines; i++)
	{
		W[i] = 2 / L[i];
		PHI[i] = S[i] * W[i];
	}
	
	//TODO 
	//in loc sa emiteti varfuri reprezentand punctele de control, emiteti varfuri care sa aproximeze curba Bezier

	for(int i=start_point; i< end_point;i++)
	{
		t = float(i) / no_of_generated_points;
		tn = float(i+1) / no_of_generated_points;
		point1 = bezier(0, t); 
		point2 = bezier(0, tn); 
		point3 = translateX(point1, width); 
		point4 = translateX(point2, width); 
		point5 = translateX(point1, -width);
		point6 = translateX(point2, -width);
		point1 = get_height(point1);
		point2 = get_height(point2);

		make_river_segment(point1, point2, point3, point4, point5, point6, t, tn);
	}


	if( show_support_points == 1)
	{
		gl_Position = Projection* View * vec4(control_p[0], 1);	EmitVertex();
		gl_Position = Projection* View * vec4(control_p[1], 1);	EmitVertex();
		gl_Position = Projection* View * vec4(control_p[2], 1);	EmitVertex();
		gl_Position = Projection* View * vec4(control_p[3], 1);	EmitVertex();
	}
	
	EndPrimitive();
}
