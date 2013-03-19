#include <iostream>
#include "grid.h"

using namespace std;
using namespace glm;

const float Grid::GRID_WIDTH = 20.0f;
const vec3 Grid::GRID_COLOR = vec3(1.0f, 1.0f, 1.0f);

Grid::Grid() : Object()
{

}

bool Grid::Initialize()
{
	if (this->GLReturnedError("Grid::Initialize - on entry"))
		return false;

	// Init a square grid
	vec3 start_x(0.0f, 0.0f, 0.0f);
	vec3 end_x(0.0f, 0.0f, GRID_WIDTH);
	vec3 start_z(0.0f, 0.0f, 0.0f);
	vec3 end_z(GRID_WIDTH, 0.0f, 0.0f);
	for (float i=0; i <= GRID_WIDTH; i++)
	{
		start_x.x = i;
		end_x.x = i;
		this->vertices.push_back(VertexAttributesPC(start_x, GRID_COLOR));
		this->vertices.push_back(VertexAttributesPC(end_x, GRID_COLOR));
		this->vertex_indices.push_back(this->vertices.size() - 2);
		this->vertex_indices.push_back(this->vertices.size() - 1);

		start_z.z = i;
		end_z.z = i;
		this->vertices.push_back(VertexAttributesPC(start_z, GRID_COLOR));
		this->vertices.push_back(VertexAttributesPC(end_z, GRID_COLOR));
		this->vertex_indices.push_back(this->vertices.size() - 2);
		this->vertex_indices.push_back(this->vertices.size() - 1);
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

	if (this->GLReturnedError("Grid::Initialize - on exit"))
		return false;

	return true;
}

void Grid::TakeDown()
{
	super::TakeDown();
}

void Grid::Draw(const mat4 & projection, mat4 modelview, Shader * shader, const ivec2 & size, const float time)
{
	if (this->GLReturnedError("Grid::Draw - on entry"))
		return;

	mat4 mvp = projection * modelview;
	mat3 nm = inverse(transpose(mat3(modelview)));

	shader->Use();
	shader->CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
	shader->CustomSetup();

	glBindVertexArray(this->vertex_array_handle);

	glDrawElements(GL_LINES, this->vertex_indices.size(), GL_UNSIGNED_INT , &this->vertex_indices[0]);

	glUseProgram(0);
	glBindVertexArray(0);

	if (this->GLReturnedError("Grid::Draw - on exit"))
		return;
}
