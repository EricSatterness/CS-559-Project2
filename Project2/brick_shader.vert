#version 400
// Modified from http://www.kickjs.org/example/shader_editor/shader_editor.html

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec3 vertex_normal;

uniform mat4 mvp;
uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;

out vec3 vColor;
out vec3 localPos;

vec4 lightPosition = vec4(0.0, 0.0, 20.0, 1.0);
vec3 La = vec3(0.1, 0.1, 0.1);
vec3 Ld = vec3(1.0, 1.0, 1.0);
vec3 Ls = vec3(1.0, 1.0, 1.0);

float shininess = 20.0;

void main()
{
	localPos = vertex_position;

	vec3 tnorm = normalize(normal_matrix * vertex_normal);
	vec4 eyeCoords = modelview_matrix * vec4(vertex_position,1.0);

	vec3 s = normalize(vec3(lightPosition - eyeCoords));
	vec3 v = normalize(-eyeCoords.xyz);
	vec3 r = reflect(-s, tnorm);

	vec3 ambient = La * vertex_color;
	float sDotN = max(dot(s,tnorm), 0.0);
	vec3 diffuse = Ld * vertex_color * sDotN;
	vec3 spec = vec3(0.0);
	if (sDotN > 0.0)
		spec = Ls * vertex_color * pow(max(dot(r,v), 0.0), shininess);

	vColor = ambient + diffuse + spec;

	gl_Position = mvp * vec4(vertex_position,1.0);
}