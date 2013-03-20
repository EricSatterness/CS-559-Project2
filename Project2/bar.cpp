#include <iostream>
#include "bar.h"

using namespace std;
using namespace glm;

const vec3 Bar::CENTER(0.0f, 0.0f, 0.0f);
const vec3 Bar::UP(0.0f, 1.0f, 0.0f);
const vec3 Bar::RIGHT(1.0f, 0.0f, 0.0f);
const float Bar::CAN_HEIGHT = 1.0f;
const float Bar::CAN_RADIUS = 0.3f;
const float Bar::BAR_HEIGHT = 29.0f;
const float Bar::BAR_WIDTH = 40.0f;
const float Bar::BAR_DEPTH = 7.0f;


Bar::Bar() : Object()
{
	this->draw_normals = false;
	this->draw_points = false;
}

void Bar::BuildNormalVisualizationGeometry()
{
	super::BuildNormalVisualizationGeometry();
}

bool Bar::Initialize()
{
	if (this->GLReturnedError("Bar::Initialize - on entry"))
		return false;

    InitBar();

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

	if (this->GLReturnedError("Bar::Initialize - on exit"))
		return false;

	return true;
}

void Bar::InitBar()
{
    int vertexRows = 10;
    int vertexCols = 10;
    int slices = 20;
    int stacks = 5;
	float w_2 = BAR_WIDTH / 2.0f;
	//float bar_offset = 30.0f;
	float bar_offset = 0.0f;
	float depth = BAR_DEPTH + bar_offset;
	//float depth = BAR_DEPTH;
	float can_depth_offset = depth + (BAR_DEPTH / 2.0f);
	float can_side_offset = BAR_WIDTH / 4.0f;
    vec3 can_color(0.0317f, 0.0620f, 0.5678f);
	vec3 bar_color(0.2450f, 0.1405f, 0.0745f);

    vec3 up_n = glm::normalize(UP);
	vec3 right_n = glm::normalize(RIGHT);
	vec3 norm_n = glm::normalize(cross(right_n, up_n));

    vec3 canMove1(can_side_offset, 0.0f, -can_depth_offset);
	vec3 canMove2(0.0f, 0.0f, -can_depth_offset);
	vec3 canMove3(-can_side_offset, 0.0f, -can_depth_offset);
    vec3 topOfCan = CENTER + (UP * (BAR_HEIGHT + CAN_HEIGHT));
	vec3 disk_up_n = -glm::normalize(cross(RIGHT, UP));

	float rotateAngle = 2.0f*PI/4.0f;
	mat3 rMatAroundUp = createRotationMatrix(up_n, rotateAngle);
	vec3 rotRight = glm::normalize(rMatAroundUp * up_n);

    //front faces
	defineRhombus(this->vertices, this->vertex_indices, vec3(-w_2, BAR_HEIGHT, -depth), up_n, right_n, BAR_HEIGHT, BAR_WIDTH, 0.0f, vertexRows, vertexCols, bar_color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(-w_2, BAR_HEIGHT, -(bar_offset + BAR_DEPTH / 2.0f)), up_n, right_n, BAR_HEIGHT * 0.07f, BAR_WIDTH, 0.0f, vertexRows, vertexCols, bar_color);
    //left faces
    defineRhombus(this->vertices, this->vertex_indices, vec3(w_2, BAR_HEIGHT, -depth), up_n, right_n * rMatAroundUp, BAR_HEIGHT, BAR_DEPTH, 0.0f, vertexRows, vertexCols, bar_color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(w_2, BAR_HEIGHT, -(bar_offset + BAR_DEPTH / 2.0f)), up_n, right_n * rMatAroundUp, BAR_HEIGHT * 0.07f, BAR_DEPTH / 2.0f, 0.0f, vertexRows, vertexCols, bar_color);
    //right faces
    defineRhombus(this->vertices, this->vertex_indices, vec3(-w_2, BAR_HEIGHT, -(depth + BAR_DEPTH)), up_n, -right_n * rMatAroundUp, BAR_HEIGHT, BAR_DEPTH, 0.0f, vertexRows, vertexCols, bar_color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(-w_2, BAR_HEIGHT, -(bar_offset + BAR_DEPTH)), up_n, -right_n * rMatAroundUp, BAR_HEIGHT * 0.07f, BAR_DEPTH / 2.0f, 0.0f, vertexRows, vertexCols, bar_color);
    //back face
    defineRhombus(this->vertices, this->vertex_indices, vec3(w_2, BAR_HEIGHT, -(depth + BAR_DEPTH)), up_n, -right_n, BAR_HEIGHT, BAR_WIDTH, 0.0f, vertexRows, vertexCols, bar_color);
    //bottom faces
    defineRhombus(this->vertices, this->vertex_indices, vec3(-w_2, 0.0f, -depth), -right_n * rMatAroundUp, right_n, BAR_DEPTH, BAR_WIDTH, 0.0f, vertexRows, vertexCols, bar_color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(-w_2, BAR_HEIGHT - (BAR_HEIGHT * 0.07f), -(bar_offset + BAR_DEPTH / 2.0f)), -right_n * rMatAroundUp, right_n, BAR_DEPTH / 2.0f, BAR_WIDTH, 0.0f, vertexRows, vertexCols, bar_color);
    //top face
    defineRhombus(this->vertices, this->vertex_indices, vec3(w_2, BAR_HEIGHT, -(depth - BAR_DEPTH / 2.0f)), -right_n * rMatAroundUp, -right_n, BAR_DEPTH * 1.5f, BAR_WIDTH, 0.0f, vertexRows, vertexCols, bar_color);
}

void Bar::TakeDown()
{
	super::TakeDown();
}

void Bar::Draw(const mat4 & projection, mat4 modelview, Shader * shader, const ivec2 & size, const float time)
{
	if (this->GLReturnedError("Bar::Draw - on entry"))
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

	if (this->GLReturnedError("Bar::Draw - on exit"))
		return;
}