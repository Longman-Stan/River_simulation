#version 430
layout(location = 0) in vec2 texture_coord;
layout(location = 1) in vec3 world_position;
layout(location = 2) in vec3 world_normal;

uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform sampler2D mountain_texture;

/*
vec3 myReflect(vec3 incident, vec3 normal)
{
	vec3 direction = normalize(reflect( incident, normal));
	vec2 uv;
	float x = direction.x;
	float y = direction.y;
	float z = direction.z;

	
	if( x >= 0 && z >= 0)
		uv[0] = 0.25 - atan( x/ z) / (2*M_PI);
	if( x <= 0 && z >= 0)
		uv[0] = 0.25 - atan( x / z) / (2*M_PI);
	if( x <= 0 && z <= 0)
		uv[0] = 0.75 - atan( x / z) / (2*M_PI);
	if( x >= 0 && z <= 0)
		uv[0] = 0.75 - atan( x / z) / (2*M_PI);
	uv[1] = acos( direction.y / sqrt( direction.x*direction.x + direction.y*direction.y + direction.z*direction.z )) / M_PI; 
	return texture(sky_texture, uv).xyz;
}
*/


layout(location = 0) out vec4 out_color;

void main()
{
	//out_color = vec4(0,1,0,1);
	/*vec3 world_norm = normalize(world_normal);

	vec3 L = normalize( light_position - world_position );
	vec3 V = normalize( eye_position - world_position );
	vec3 H = normalize( L + V );
	
	float ambient_light = 0.7;
	float diffuse_light = material_kd * max ( dot(world_norm,L), 0);

	float primesteLumina;
	if( dot(world_normal,L) > 0 )
		primesteLumina = 1;
	else 
		primesteLumina = 0;

	float specular_light = 0;

	float d = distance(world_position,light_position);
	float atenuation = 1;

	float light=ambient_light;

	if (diffuse_light > 0)
	{
		specular_light = material_ks * primesteLumina * pow(max(dot(world_norm, H), 0), material_shininess);
	}

	light += atenuation*diffuse_light;
	light += atenuation*specular_light;*/
	

	out_color = texture(mountain_texture, texture_coord);
	//vec3(0,0.53,0.55)
	//out_color = vec4( (water_color*0.7 + reflect_color*0.3) * light, 1);

	//vec4 water_col = texture(water_texture, texture_coord);
	//out_color = water_col; //vec4( texture_coord.x, texture_coord.y, 0, 1);
}