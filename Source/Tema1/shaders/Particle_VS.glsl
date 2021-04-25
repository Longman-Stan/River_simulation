#version 430

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform vec3 position;
uniform vec3 target;
uniform float dt;

struct Particle
{
	vec4 position;
	vec4 speed;
	vec4 iposition;
	vec4 ispeed;
};

layout(std430, binding = 0) buffer particles {
	Particle data[];
};

float rand(vec2 co)
{
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec3 pos = data[gl_VertexID].position.xyz;
	vec3 spd = data[gl_VertexID].speed.xyz;

	pos = pos + spd * dt + vec3(0, -1.9, 0) * dt * dt/2.0f ;
	spd = spd + vec3(0, -1.9, 0) * dt;

	if(distance(pos,position) > 16 || pos.y < 0)
	{
		vec3 dir = target - position;
		pos = position - dir * 0.4;
		spd = data[gl_VertexID].ispeed.xyz;
		spd.x *= dir.x*12;
		spd.z *= dir.z*12;
	}

	data[gl_VertexID].position.xyz =  pos;
	data[gl_VertexID].speed.xyz =  spd;

	gl_Position = Model * vec4(pos, 1);
}


