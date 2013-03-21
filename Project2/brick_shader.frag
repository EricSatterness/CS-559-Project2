#version 400
// Modified from http://www.kickjs.org/example/shader_editor/shader_editor.html

//vec3 BrickColor(0.502f, 0.314f, 0.055f);
//vec3 MortarColor(0.553f, 0.553f, 0.553f);
//vec2 BrickSize(4.0f, 2.0f);
//vec2 BrickPct(0.0f, 0.0f);
//vec3 BrickColor = vec3(1.0f, 0.3f, 0.2f);
//vec3 MortarColor = vec3(0.85f, 0.86f, 0.84f);
//vec2 BrickSize = vec2(0.3f, 0.15f);
//vec2 BrickPct = vec2(0.9f, 0.85f);

layout (location = 0) out vec4 FragColor;

in vec3 vColor;
in vec3 localPos;

vec3 BrickColor = vec3(1.0f, 0.3f, 0.2f);
vec3 MortarColor = vec3(0.85f, 0.86f, 0.84f);
vec3 BrickSize = vec3(2.25f, 1.15f, 2.25f);
vec3 BrickPct = vec3(0.89f, 0.85f, 0.85f);

void main()
{
    vec3 color;
	vec3 position, useBrick;
	

	position = localPos / BrickSize.xyz;

	if (fract(position.y * 0.5) > 0.5){
		position.x += 0.5;
        position.z += 0.5;
	}
    
	position = fract(position);

	useBrick = step(position, BrickPct.xyz);

	color = mix(MortarColor, BrickColor, useBrick.x * useBrick.y * useBrick.z);
	color *= vColor;

	FragColor = vec4(color, 1.0);
}