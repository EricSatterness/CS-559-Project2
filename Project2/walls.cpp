#include <iostream>
#include "Walls.h"

using namespace std;
using namespace glm;

//#define DRAW_POINTS (1)

const vec3 Walls::CENTER(0.0f, 0.0f, 0.0f);
const vec3 Walls::UP(0.0f, 1.0f, 0.0f);
const vec3 Walls::RIGHT(1.0f, 0.0f, 0.0f);
const float Walls::WALLS_HEIGHT = 50.0f;
const float Walls::WALLS_WIDTH = 100.0f;


Walls::Walls() : Object()
{
	this->draw_normals = false;
	this->draw_points = false;
}

void Walls::BuildNormalVisualizationGeometry()
{
	super::BuildNormalVisualizationGeometry();
}

bool Walls::Initialize()
{
	if (this->GLReturnedError("Walls::Initialize - on entry"))
		return false;

    InitWalls();

	this->BuildNormalVisualizationGeometry();

	if (!this->PostGLInitialize(&this->vertex_array_handle, &this->vertex_coordinate_handle, this->vertices.size() * sizeof(VertexAttributesPCNT), &this->vertices[0]))
		return false;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesPCNT), (GLvoid *) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesPCNT), (GLvoid *) (sizeof(vec3)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesPCNT), (GLvoid *) (sizeof(vec3) * 2));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesPCNT), (GLvoid *) (sizeof(vec3) * 3));

	// Each of the attributes to be used must be enabled.
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	if (this->normal_vertices.size() > 0)
	{
		if (!this->PostGLInitialize(&this->normal_array_handle, &this->normal_coordinate_handle, this->normal_vertices.size() * sizeof(VertexAttributesP), &this->normal_vertices[0]))
			return false;

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesP), (GLvoid *) 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	if (this->GLReturnedError("Walls::Initialize - on exit"))
		return false;

	return true;
}

void Walls::InitWalls()
{
    int vertexRows = 100;
	int vertexCols = 100;
	float w_2 = WALLS_WIDTH / 2.0f;
    //vec3 color(0.3141f, 0.4626f, 0.0628f);
	vec3 color(0.81f, 0.81f, 0.81f);

    vec3 up_n = glm::normalize(UP);
	vec3 right_n = glm::normalize(RIGHT);
	vec3 norm_n = glm::normalize(cross(right_n, up_n));
	float rotateAngle = 2.0f*PI/4.0f;
	mat3 rMatAroundUp = createRotationMatrix(up_n, rotateAngle);
	mat3 rMatAroundUp2 = createRotationMatrix(up_n, -PI/2.0f);
	vec3 rotRight = glm::normalize(rMatAroundUp2 * right_n);

    defineRhombus(this->vertices, this->vertex_indices, vec3(w_2, WALLS_HEIGHT, w_2), up_n, -right_n, WALLS_HEIGHT, WALLS_WIDTH, 0.0f, vertexRows, vertexCols, color);
    defineRhombus(this->vertices, this->vertex_indices, vec3(-w_2, WALLS_HEIGHT, w_2), up_n, right_n * rMatAroundUp, WALLS_HEIGHT, WALLS_WIDTH, 0.0f, vertexRows, vertexCols, color);
    defineRhombus(this->vertices, this->vertex_indices, vec3(-w_2, WALLS_HEIGHT, -w_2), up_n, right_n, WALLS_HEIGHT, WALLS_WIDTH, 0.0f, vertexRows, vertexCols, color);
    defineRhombus(this->vertices, this->vertex_indices, vec3(w_2, WALLS_HEIGHT, -w_2), up_n, right_n * -rMatAroundUp, WALLS_HEIGHT, WALLS_WIDTH, 0.0f, vertexRows, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(-w_2, 0.0f, w_2), -right_n, rotRight, WALLS_HEIGHT * 2.0f, WALLS_WIDTH, 0.0f, vertexRows, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(-w_2, WALLS_HEIGHT, w_2), -rotRight, right_n, WALLS_HEIGHT * 2.0f, WALLS_WIDTH, 0.0f, vertexRows, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(-w_2, 0.0f, w_2), -rotRight, right_n, WALLS_HEIGHT * 2.0f, WALLS_WIDTH, 0.0f, vertexRows, vertexCols, color);
}



void Walls::TakeDown()
{
	super::TakeDown();
}

void Walls::Draw(const mat4 & projection, mat4 modelview, Shader * shader, const ivec2 & size, const float time)
{
	if (this->GLReturnedError("Walls::Draw - on entry"))
		return;

	mat4 mvp = projection * modelview;
	mat3 nm = inverse(transpose(mat3(modelview)));

	shader->Use();
	shader->CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
	shader->CustomSetup();

	glBindVertexArray(this->vertex_array_handle);

	if (this->draw_points)
		glDrawArrays(GL_POINTS, 0, this->vertex_indices.size());
	else
		glDrawElements(GL_TRIANGLES, this->vertex_indices.size(), GL_UNSIGNED_INT , &this->vertex_indices[0]);

	glUseProgram(0);
	glBindVertexArray(0);

	if (this->draw_normals)
	{
		shader->Use();
		shader->CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
		shader->CustomSetup();

		glBindVertexArray(this->normal_array_handle);
		glDrawElements(GL_LINES , this->normal_indices.size(), GL_UNSIGNED_INT , &this->normal_indices[0]);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	if (this->GLReturnedError("Walls::Draw - on exit"))
		return;
}