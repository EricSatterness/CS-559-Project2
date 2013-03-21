#include <iostream>
#include "axes.h"

using namespace std;
using namespace glm;

const vec3 Axes::ORIGIN = vec3(0.0f,0.0f,0.0f);
const vec3 Axes::X_AXIS = vec3(1.0f,0.0f,0.0f);
const vec3 Axes::Y_AXIS = vec3(0.0f,1.0f,0.0f);
const vec3 Axes::Z_AXIS = vec3(0.0f,0.0f,1.0f);

Axes::Axes() : Object()
{

}

bool Axes::Initialize()
{
	if (this->GLReturnedError("Axes::Initialize - on entry"))
		return false;

	// Init axes. Note: The color of each axis is the same as it's point vector
	this->vertices.push_back(VertexAttributesPC(ORIGIN, X_AXIS));
	this->vertices.push_back(VertexAttributesPC(X_AXIS, X_AXIS));
	this->vertices.push_back(VertexAttributesPC(ORIGIN, Y_AXIS));
	this->vertices.push_back(VertexAttributesPC(Y_AXIS, Y_AXIS));
	this->vertices.push_back(VertexAttributesPC(ORIGIN, Z_AXIS));
	this->vertices.push_back(VertexAttributesPC(Z_AXIS, Z_AXIS));

	for (int i = 0; i < (int)this->vertices.size(); i++)
	{
		this->vertex_indices.push_back(i);
	}

	if (!this->PostGLInitialize(&this->vertex_array_handle, &this->vertex_coordinate_handle, this->vertices.size() * sizeof(VertexAttributesPC), &this->vertices[0]))
		return false;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesPC), (GLvoid *) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesPC), (GLvoid *) (sizeof(vec3)));

	// Each of the attributes to be used must be enabled.
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	if (this->GLReturnedError("Axes::Initialize - on exit"))
		return false;

	return true;
}

void Axes::TakeDown()
{
	super::TakeDown();
}

void Axes::Draw(const mat4 & projection, mat4 modelview, Shader * shader, const ivec2 & size, const float time)
{
	if (this->GLReturnedError("Axes::Draw - on entry"))
		return;

	mat4 mvp = projection * modelview;
	mat3 nm = inverse(transpose(mat3(modelview)));

	shader->Use();
	shader->CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
	shader->CustomSetup();

	glBindVertexArray(this->vertex_array_handle);

	//GLfloat prevLineWidth = GL_LINE_WIDTH;
	glLineWidth(2.0);
	glDrawElements(GL_LINES, this->vertex_indices.size(), GL_UNSIGNED_INT , &this->vertex_indices[0]);
	//glLineWidth(prevLineWidth);

	glUseProgram(0);
	glBindVertexArray(0);

	if (this->GLReturnedError("Axes::Draw - on exit"))
		return;
}
