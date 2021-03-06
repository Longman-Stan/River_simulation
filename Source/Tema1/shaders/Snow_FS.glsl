#version 430

layout(location = 0) in vec2 texture_coord;

uniform sampler2D snow_particle;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = texture(snow_particle, texture_coord);
	if(out_color.a == 0)
		discard;
}