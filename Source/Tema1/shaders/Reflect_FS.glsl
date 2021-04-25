#version 430

layout(location = 0) in vec3 world_position;
layout(location = 1) in vec3 world_normal;

uniform sampler2D texture_1;
uniform samplerCube texture_cubemap;
uniform sampler2D sky_texture;

uniform vec3 camera_position;

layout(location = 0) out vec4 out_color;

vec3 myReflect()
{
    // TODO - compute the reflection color value
	vec3 incident = normalize(world_position - camera_position);
	vec3 direction = reflect( incident, world_normal);
	return texture(texture_cubemap, direction).xyz;
}

#define M_PI 3.1415926535897932384626433832795
vec3 myReflect2()
{
	vec2 uv;
	vec3 incident = normalize(world_position - camera_position);
	vec3 direction = reflect( incident, world_normal);
	uv[0] = atan( direction.z / direction.x) / (2*M_PI);
	uv[1] = acos( direction.y / sqrt( direction.x*direction.x + direction.y*direction.y + direction.z*direction.z )) / M_PI;  
	//uv[0] = ( atan( direction.x / direction.y) + M_PI) / (2*M_PI);
	//uv[1] = ( 2 *asin(direction.z) + M_PI) / ( 2 * M_PI);
	return texture(sky_texture, uv).xyz;
}

void main()
{
	//out_color = vec4(myReflect2(), 1);
	out_color = vec4(world_normal,1);
}
