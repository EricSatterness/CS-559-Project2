/*
Perry Kivolowitz - University of Wisconsin - Madison 
Computer Sciences Department

A sample hello world like program demonstrating modern
OpenGL techniques. 

Created:        2/25/13
Updates:
*/

#pragma once
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vertexattributes.h"

class Object
{
public:
	Object();

	virtual void TakeDown();
	virtual bool Initialize();
	virtual bool PostGLInitialize(GLuint * vertex_array_handle, GLuint * vertex_coordinate_handle, GLsizeiptr sz, const GLvoid * ptr);
	virtual void Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size, const float time = 0) = 0;
	virtual ~Object();

	inline void EnableNormals(bool dn) { this->draw_normals = dn; }
	inline void EnablePoints(bool dp) { this->draw_points = dp; }

protected:
	bool draw_normals, draw_points;
	GLuint vertex_coordinate_handle;
	GLuint vertex_array_handle;
	GLuint normal_coordinate_handle;
	GLuint normal_array_handle;

	bool GLReturnedError(char * s);
	std::vector<VertexAttributesPCNT> vertices;
	std::vector<GLuint> vertex_indices;
	std::vector<VertexAttributesP> normal_vertices;
	std::vector<GLuint> normal_indices;

private:
	void InternalInitialize();
};