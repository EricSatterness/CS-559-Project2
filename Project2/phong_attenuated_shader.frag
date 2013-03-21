#version 400

layout (location = 0) out vec4 FragColor;

flat in vec3 color;
in vec3 position;
in vec3 normal;

vec4 lightPosition = vec4(0.0, 0.0, 20.0, 1.0);
vec3 La = vec3(0.1, 0.1, 0.1);
vec3 Ld = vec3(1.0, 1.0, 1.0);
vec3 Ls = vec3(1.0, 1.0, 1.0);
float shininess = 20.0;

// Attenuation factors
float D;
float c = 1.0f;
float l = 0.05f;
float q = 0.0f;

void main()
{
	vec3 n = normalize(normal);
	vec3 s = normalize(vec3(lightPosition) - position);
	vec3 v = normalize(-position.xyz);
	vec3 r = reflect(-s, n);

	D = abs(length(vec3(lightPosition) - position));

	vec3 ambient = La * color;
	float sDotN = max(dot(s,n), 0.0);
	//float sDotN = dot(s,n);
	vec3 diffuse = Ld * color * sDotN;
	vec3 spec = vec3(0.0);
	if (sDotN > 0.0)
		spec = Ls * color * pow(max(dot(r,v), 0.0), shininess);
	//vec3 spec = Ls * color * pow(dot(r,v), shininess);

	vec3 lightIntensity = ambient + (diffuse + spec) * (1 / (c + l*D + q*pow(D,2.0)));
	FragColor = vec4(lightIntensity, 1.0);
}