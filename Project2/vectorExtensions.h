#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

using namespace std;
using namespace glm;

mat3 createRotationMatrix(vec3 axis, float angle);
mat4 createRotationMatrix(vec4 axis, float angle);