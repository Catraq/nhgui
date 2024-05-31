#version 430 core

uniform sampler2D in_texture;

uniform uvec2 dimension;
uniform vec2 position;
uniform vec2 size;

in vec2 fragcoord;
out vec4 color;

void main()
{

	vec2 dim = vec2(dimension);
	vec2 uv = vec2(gl_FragCoord.xy)/dim;
	vec4 s = vec4(1,1,1, texture(in_texture, fragcoord).r);
	color = vec4(0.0, 0.0, 0.0, 1.0) * s;
}


