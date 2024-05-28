#version 430 core

#define RADIO_BUTTON_TICKNESS radius*radius/4


uniform uint checked;
uniform uvec2 dimension;

in vec2 center;
in float radius;


out vec4 color;

void main()
{

	vec2 dim = vec2(dimension);
	vec2 uv = 2.0*vec2(gl_FragCoord.xy)/dim-1.0;
	
	vec2 d = uv-center;
	d.x *= dim.x/dim.y;
	float r = radius * dim.x/dim.y;

	
	
	float v = dot(d,d);

	if(v < r*r)
	{
		color = vec4(0.7, 0.7, 0.7, 0);
		
		if(v < r * r - r*r/2)
		{
			color = vec4(1, 1, 1, 0);
			
			if(v < r * r - r*r * 1.5/2 && checked > 0)
			{
				color = vec4(0, 0, 0, 0);
			}
		}
	}
	else
		discard;

}


