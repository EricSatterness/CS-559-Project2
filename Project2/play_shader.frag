#version 400

layout (location = 0) out vec4 FragColor;

uniform float time;
uniform ivec2 size;

in vec2 tc;

void main()
{
	float f = gl_FragCoord.x / float(size.x);
	FragColor = vec4(f, f, f, 1);
}
