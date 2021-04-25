#version 430

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;


layout(location = 0) out vec2 texture_coord;

void main()
{
	vec3 new_position =  v_position;

	texture_coord = v_texture_coord;

	gl_Position = Projection * View * Model * vec4(new_position, 1);
}