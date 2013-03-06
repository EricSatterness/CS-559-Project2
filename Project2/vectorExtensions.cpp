#include "vectorExtensions.h"

// Create a rotation matrix that will rotate a vector around the specified axis
//http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle

mat3 createRotationMatrix(vec3 axis, float angle)
{
	float c = cosf(angle);
	float iC = 1 - cosf(angle);
	float s = sinf(angle);
	float iS = 1 - sinf(angle);
	mat3 rMat = mat3(vec3((c + pow(axis.x, 2.0f) * iC), (axis.x * axis.y * iC - axis.z * s), (axis.x * axis.z * iC + axis.y * s)),
					vec3((axis.y * axis.x * iC + axis.z * s), (c + pow(axis.y, 2.0f) * iC), (axis.y * axis.z * iC - axis.x * s)),
					vec3((axis.z * axis.x * iC - axis.y * s), (axis.z * axis.y * iC + axis.x * s), (c + pow(axis.z, 2.0f) * iC)));

	return rMat;
}

mat4 createRotationMatrix(vec4 axis, float angle)
{
	float c = cosf(angle);
	float iC = 1 - cosf(angle);
	float s = sinf(angle);
	float iS = 1 - sinf(angle);
	mat4 rMat = mat4(vec4((c + pow(axis.x, 2.0f) * iC), (axis.x * axis.y * iC - axis.z * s), (axis.x * axis.z * iC + axis.y * s), (0.0f)),
					vec4((axis.y * axis.x * iC + axis.z * s), (c + pow(axis.y, 2.0f) * iC), (axis.y * axis.z * iC - axis.x * s), (0.0f)),
					vec4((axis.z * axis.x * iC - axis.y * s), (axis.z * axis.y * iC + axis.x * s), (c + pow(axis.z, 2.0f) * iC), (0.0f)),
					vec4(0.0f, 0.0f, 0.0f, 1.0f));

	return rMat;
}