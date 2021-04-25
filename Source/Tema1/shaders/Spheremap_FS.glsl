#version 430
layout(location = 0) in vec2 texture_coord;

//Uniform texture normal
uniform sampler2D sky_texture;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = texture(sky_texture, texture_coord);
}
