#version 400

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec3 vertex_normal;
layout (location = 3) in vec2 texture_coordinates;

uniform mat4 mvp;
uniform mat4 modelview_matrix;
uniform mat4 projection_matrix;
uniform mat4 normal_matrix;

uniform float time;
uniform ivec2 size;

out vec2 tc;

void main()
{
	tc = texture_coordinates;
	gl_Position = mvp * vec4(vertex_position, 1.0);
}
