#version 400

in vec3 lightIntensity;

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(lightIntensity,1.0);
}