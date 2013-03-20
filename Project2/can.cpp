#include <iostream>
#include "can.h"

using namespace std;
using namespace glm;

//#define DRAW_POINTS (1)

const vec3 Can::CENTER(0.0f, 0.0f, 0.0f);
const vec3 Can::UP(0.0f, 1.0f, 0.0f);
const vec3 Can::RIGHT(1.0f, 0.0f, 0.0f);
const float Can::CAN_HEIGHT = 1.0f;
const float Can::CAN_RADIUS = 0.3f;
const float Can::BAR_HEIGHT = 29.0f;
const float Can::BAR_WIDTH = 40.0f;
const float Can::BAR_DEPTH = 7.0f;


Can::Can() : Object()
{
	this->draw_normals = false;
	this->draw_points = false;
}

void Can::BuildNormalVisualizationGeometry()
{
	super::BuildNormalVisualizationGeometry();
}

bool Can::Initialize()
{
	if (this->GLReturnedError("Bar::Initialize - on entry"))
		return false;

    InitCan();

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

void Can::InitCan()
{
    int vertexRows = 10;
    int vertexCols = 10;
    int slices = 20;
    int stacks = 5;
	float w_2 = BAR_WIDTH / 2.0f;
	//float bar_offset = 30.0f;
	float bar_offset = 0.0f;
	float depth = BAR_DEPTH + bar_offset;
	float can_depth_offset = depth + (BAR_DEPTH / 2.0f);
	float can_side_offset = BAR_WIDTH / 4.0f;
    vec3 can_color(0.0317f, 0.0620f, 0.5678f);
	vec3 bar_color(0.2450f, 0.1405f, 0.0745f);

    vec3 up_n = glm::normalize(UP);
	vec3 right_n = glm::normalize(RIGHT);
	vec3 norm_n = glm::normalize(cross(right_n, up_n));

    vec3 canMove1(0.0f, 0.0f, -can_depth_offset);
	//vec3 canMove2(0.0f, 0.0f, -can_depth_offset);
	//vec3 canMove3(-can_side_offset, 0.0f, -can_depth_offset);
    vec3 topOfCan = CENTER + (UP * (BAR_HEIGHT + CAN_HEIGHT));
	vec3 disk_up_n = -glm::normalize(cross(RIGHT, UP));

	float rotateAngle = 2.0f*PI/4.0f;
	mat3 rMatAroundUp = createRotationMatrix(up_n, rotateAngle);
	vec3 rotRight = glm::normalize(rMatAroundUp * up_n);

    //beer can 1
    defineDisk(this->vertices, this->vertex_indices, topOfCan + canMove1, disk_up_n, RIGHT, CAN_RADIUS, slices, can_color);
	defineCylinder(this->vertices, this->vertex_indices, topOfCan + canMove1, UP, RIGHT, CAN_RADIUS, CAN_RADIUS, CAN_HEIGHT, slices, stacks, can_color);
	defineDisk(this->vertices, this->vertex_indices, topOfCan - (UP * CAN_HEIGHT) + canMove1, -disk_up_n, RIGHT, CAN_RADIUS, slices, can_color);
    /*
	//beer can 2
    defineDisk(this->vertices, this->vertex_indices, topOfCan + canMove2, disk_up_n, RIGHT, CAN_RADIUS, slices, can_color);
	defineCylinder(this->vertices, this->vertex_indices, topOfCan + canMove2, UP, RIGHT, CAN_RADIUS, CAN_RADIUS, CAN_HEIGHT, slices, stacks, can_color);
	defineDisk(this->vertices, this->vertex_indices, topOfCan - (UP * CAN_HEIGHT) + canMove2, -disk_up_n, RIGHT, CAN_RADIUS, slices, can_color);
    //beer can 3
    defineDisk(this->vertices, this->vertex_indices, topOfCan + canMove3, disk_up_n, RIGHT, CAN_RADIUS, slices, can_color);
	defineCylinder(this->vertices, this->vertex_indices, topOfCan + canMove3, UP, RIGHT, CAN_RADIUS, CAN_RADIUS, CAN_HEIGHT, slices, stacks, can_color);
	defineDisk(this->vertices, this->vertex_indices, topOfCan - (UP * CAN_HEIGHT) + canMove3, -disk_up_n, RIGHT, CAN_RADIUS, slices, can_color);
	*/
}

void Can::TakeDown()
{
	super::TakeDown();
}

void Can::Draw(const mat4 & projection, mat4 modelview, Shader * shader, const ivec2 & size, const float time)
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

	if (this->GLReturnedError("Can::Draw - on exit"))
		return;
}