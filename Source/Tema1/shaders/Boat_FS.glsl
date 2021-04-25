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


layout(location = 0) out vec4 out_color;

void main()
{
	//out_color = vec4(0,1,0,1);
	vec3 world_norm = normalize(world_normal);

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
	light += atenuation*specular_light;
	
	vec3 boat_color = vec3(0.38, 0.2, 0.07) * light;

	out_color = vec4(boat_color,1);
}