#version 430
layout(location = 0) in vec2 texture_coord;

uniform sampler2D textColor;
uniform sampler2D textDepth;

uniform ivec2 screenSize;
uniform float fov_start;
uniform float fov_end;
uniform float near;
uniform float far;

layout(location = 0) out vec4 out_color;

vec4 blur(int blurRadius)
{
	vec2 texelSize = 1.0f / screenSize;
	vec4 sum = vec4(0);
	for(int i = -blurRadius; i <= blurRadius; i++)
	{
		for(int j = -blurRadius; j <= blurRadius; j++)
		{
			sum += texture(textColor, texture_coord + vec2(i, j) * texelSize);
		}
	}
		
	float samples = pow((2 * blurRadius + 1), 2);
	return sum / samples;
}

float LinearizeDepth()
{
  float z = 2.0 * texture(textDepth, texture_coord).x - 1.0;
  return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
	vec4 color = texture(textColor, texture_coord);
	vec4 blurred = blur(1);

	float depth = LinearizeDepth();

	float factor;
	if( depth < fov_start)
	{
		factor = (fov_start - depth) / (fov_start - near);
		color = color * factor + blurred * (1 - factor);
	}
	if( depth > fov_end)
	{
		//factor = (depth - fov_end) / (far - fov_end);
		factor = min( log(depth-fov_end+1), 1);
		color = color * (1-factor) + blurred * factor;
	}
	out_color = vec4(color.xyz,1);
	
	/*if( depth > fov_end)
	{
		out_color = vec4( vec3(1),1);
	}
	else
	{
		out_color = vec4( vec3(depth) / (far-near),1);
	}*/

	//out_color = vec4( vec3(depth) / (far-near) ,1);

	out_color = vec4( color.xyz, 1);
}
