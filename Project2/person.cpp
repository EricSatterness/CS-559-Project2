#include <iostream>
#include "Person.h"


const vec3 Person::CENTER(0.0f, 0.0f, 0.0f);
const vec3 Person::UP(0.0f, 1.0f, 0.0f);
const vec3 Person::RIGHT(1.0f, 0.0f, 0.0f);
const float Person::LEG_WIDTH = 1.606f;
const float Person::LEG_HEIGHT = 22.167f;
const float Person::BAR_HEIGHT = 29.0f;
const float Person::BAR_WIDTH = 40.0f;
const float Person::BAR_DEPTH = 7.0f;
const float Person::SPINE_HEIGHT = 20.0f;
const float Person::HEAD_LENGTH = 2.0f;
const float Person::HEAD_RADIUS = 2.5f;

Person::Person() : Object()
{
	this->draw_normals = false;
	this->draw_points = false;
}

void Person::BuildNormalVisualizationGeometry()
{
	super::BuildNormalVisualizationGeometry();
}

bool Person::Initialize()
{
	if (this->GLReturnedError("Environment::Initialize - on entry"))
		return false;

    InitPerson();

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

	if (this->GLReturnedError("Environment::Initialize - on exit"))
		return false;

	return true;
}

void Person::InitPerson()
{
    int vertexRows = 10;
    int vertexCols = 10;
    int slices = 30;
    int stacks = 5;
    float angle = 15.0f*PI/180.0f;
	float w_2 = LEG_WIDTH / 2.0f;
	float h_2 = SPINE_HEIGHT / 2.0f;
	float bar_offset = 30.0f;
	float bartender_offset = bar_offset + BAR_DEPTH * 2.5f;
	float bartender_height = LEG_HEIGHT * 0.75f;
	float tender_top = SPINE_HEIGHT + bartender_height + HEAD_RADIUS;
	float leg_offset = LEG_HEIGHT * 0.375f;
    vec3 color(1.0f, 1.0f, 1.0f);

    vec3 up_n = glm::normalize(UP);
	vec3 right_n = glm::normalize(RIGHT);

    vec3 canMove(0.0f, 0.0f, 2.0f);
	vec3 disk_up_n = -glm::normalize(cross(RIGHT, UP));

    mat3 rMatAroundRight = createRotationMatrix(right_n, angle);
	mat3 rMatAroundUp = createRotationMatrix(up_n, -PI/2.0f);
	vec3 tiltedUp = glm::normalize(rMatAroundRight * up_n);
	vec3 rotRight = glm::normalize(rMatAroundUp * right_n);

    //head -- need to rotate so it's like a can on it's side
	defineDisk(this->vertices, this->vertex_indices, vec3(0.0f, tender_top, -bartender_offset), up_n, right_n, HEAD_RADIUS, slices, color);
	defineCylinder(this->vertices, this->vertex_indices, vec3(0.0f, tender_top, -bartender_offset), -rotRight, up_n, HEAD_RADIUS, HEAD_RADIUS, HEAD_LENGTH, slices, stacks, color);
	defineDisk(this->vertices, this->vertex_indices, vec3(0.0f, tender_top, -bartender_offset - HEAD_LENGTH), up_n, -right_n, HEAD_RADIUS, slices, color);

    //spine
    defineRhombus(this->vertices, this->vertex_indices, vec3(-LEG_WIDTH / 2.0f, SPINE_HEIGHT + bartender_height, -bartender_offset), up_n, right_n, SPINE_HEIGHT, LEG_WIDTH, 0.0f, vertexRows, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(LEG_WIDTH / 2.0f, SPINE_HEIGHT + bartender_height, -bartender_offset), up_n, rotRight, SPINE_HEIGHT, LEG_WIDTH, 0.0f, vertexRows, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(LEG_WIDTH / 2.0f, SPINE_HEIGHT + bartender_height, -bartender_offset - LEG_WIDTH), up_n, -right_n, SPINE_HEIGHT, LEG_WIDTH, 0.0f, vertexRows, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(-LEG_WIDTH / 2.0f, SPINE_HEIGHT + bartender_height, -bartender_offset - LEG_WIDTH), up_n, -rotRight, SPINE_HEIGHT, LEG_WIDTH, 0.0f, vertexRows, vertexCols, color);
	// Top and Bottom
	defineRhombus(this->vertices, this->vertex_indices, vec3(-LEG_WIDTH / 2.0f, SPINE_HEIGHT + bartender_height, -bartender_offset), -right_n, rotRight, LEG_WIDTH, LEG_WIDTH, 0.0f, vertexCols, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(-LEG_WIDTH / 2.0f, bartender_height, -bartender_offset), -rotRight, right_n, LEG_WIDTH, LEG_WIDTH, 0.0f, vertexCols, vertexCols, color);

	//left arm
	InitArm(vec3(-LEG_WIDTH * 1.5f, SPINE_HEIGHT / 2.0f + bartender_height, -bartender_offset - LEG_WIDTH / 2.0f), UP, vec3(0.0f, 0.0f, 1.0f));
	//right arm
	InitArm(vec3(LEG_WIDTH * 1.5f, SPINE_HEIGHT / 2.0f + bartender_height, -bartender_offset - LEG_WIDTH / 2.0f), UP, vec3(0.0f, 0.0f, -1.0f));
	//left leg
	InitArm(vec3(-LEG_WIDTH * 1.5f, leg_offset, -bartender_offset - LEG_WIDTH / 2.0f), UP, vec3(0.0f, 0.0f, 1.0f));
	//right leg
	InitArm(vec3(LEG_WIDTH * 1.5f, leg_offset, -bartender_offset - LEG_WIDTH / 2.0f), UP, vec3(0.0f, 0.0f, -1.0f));
    
}

void Person::InitArm(vec3 center, vec3 up, vec3 right)
{
	int vertexRows = 10;
	int vertexCols = 2;
	float arm_height = LEG_HEIGHT * 0.75f;
	vec3 color(1.0f, 1.0f, 1.0f);

	vec3 up_n = glm::normalize(up);
	vec3 right_n = glm::normalize(right);
	vec3 norm_n = glm::normalize(cross(right_n, up_n));
	float w_2 = LEG_WIDTH / 2.0f;
	float h_2 = arm_height / 2.0f;
	float angle = 15.0f*PI/180.0f;

	mat3 rMatAroundRight = createRotationMatrix(right_n, angle);
	mat3 rMatAroundUp = createRotationMatrix(up_n, -PI/2.0f);
	vec3 tiltedUp = glm::normalize(rMatAroundRight * up_n);
	vec3 rotRight = glm::normalize(rMatAroundUp * right_n);

	defineRhombus(this->vertices, this->vertex_indices, vec3(center - w_2*right_n + h_2*up_n + (-h_2*tanf(angle) + w_2)*norm_n), tiltedUp, right_n, arm_height/sin(PI/2 - angle), LEG_WIDTH, 0.0f, vertexRows, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(center + w_2*right_n + h_2*up_n + (-h_2*tanf(angle) + w_2)*norm_n), up_n, rotRight, arm_height, LEG_WIDTH, -angle, vertexRows, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(center + w_2*right_n + h_2*up_n + (-h_2*tanf(angle) - w_2)*norm_n), tiltedUp, -right_n, arm_height/sin(PI/2 - angle), LEG_WIDTH, 0.0f, vertexRows, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(center - w_2*right_n + h_2*up_n + (-h_2*tanf(angle) - w_2)*norm_n), up_n, -rotRight, arm_height, LEG_WIDTH, angle, vertexRows, vertexCols, color);
	// Top and Bottom
	defineRhombus(this->vertices, this->vertex_indices, vec3(center - w_2*right_n + h_2*up_n + (-h_2*tanf(angle) + w_2)*norm_n), -right_n, rotRight, LEG_WIDTH, LEG_WIDTH, 0.0f, vertexCols, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(center - w_2*right_n - h_2*up_n + (h_2*tanf(angle) + w_2)*norm_n), -rotRight, right_n, LEG_WIDTH, LEG_WIDTH, 0.0f, vertexCols, vertexCols, color);
}

void Person::TakeDown()
{
	super::TakeDown();
}

void Person::Draw(const mat4 & projection, mat4 modelview, Shader * shader, const ivec2 & size, const float time)
{
	if (this->GLReturnedError("Environment::Draw - on entry"))
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

	if (this->GLReturnedError("Environment::Draw - on exit"))
		return;
}