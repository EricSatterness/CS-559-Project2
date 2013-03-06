#pragma once
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vertexattributes.h"
#include "math.h"

#define PI 3.14159265f

using namespace std;
using namespace glm;

// A collection of functions for generating shapes. Each function specifies the vertices and their indices for drawing triangles
void defineVertexIndices(vector<GLuint> & vertex_indices, int cols, int rows, int startPos);
//void defineRectangle(vector<VertexAttributes> & vertices, vector<GLuint> & vertex_indices, vec3 topleft, int height, int width);
//void defineCube(vector<VertexAttributes> & vertices, vector<GLuint> & vertex_indices, vec3 center, int height, int width);
void defineRhombus(vector<VertexAttributes> & vertices, vector<GLuint> & vertex_indices, vec3 topleft, vec3 up, vec3 right, float height, float width, float angle, int vertexRows, int vertexCols);