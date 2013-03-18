#include <iostream>
#include "environment.h"

using namespace std;
using namespace glm;

//#define DRAW_POINTS (1)

const vec3 Environment::CENTER(0.0f, 0.0f, 0.0f);
const vec3 Environment::UP(0.0f, 1.0f, 0.0f);
const vec3 Environment::RIGHT(1.0f, 0.0f, 0.0f);
const float Environment::LEG_WIDTH = 1.606f;
const float Environment::LEG_HEIGHT = 22.167f;
const float Environment::CAN_HEIGHT = 1.0f;
const float Environment::CAN_RADIUS = 0.3f;
const float Environment::BAR_HEIGHT = 29.0f;
const float Environment::BAR_WIDTH = 40.0f;
const float Environment::BAR_DEPTH = 7.0f;
const float Environment::SPINE_HEIGHT = 20.0f;
const float Environment::HEAD_LENGTH = 2.0f;
const float Environment::HEAD_RADIUS = 2.5f;
const float Environment::WALLS_HEIGHT = 50.0f;
const float Environment::WALLS_WIDTH = 100.0f;


Environment::Environment() : Object()
{
	//this->shader_index = 0;
	this->draw_normals = false;
	this->draw_points = false;
}

void Environment::BuildNormalVisualizationGeometry()
{
	float normal_scalar = 1.0f;
	for (int i = 0; i < int(this->vertices.size()); i++)
	{
		this->normal_vertices.push_back(VertexAttributesP(this->vertices[i].position));
		this->normal_vertices.push_back(VertexAttributesP(this->vertices[i].position + this->vertices[i].normal * normal_scalar));
		this->normal_indices.push_back(this->normal_vertices.size() - 2);
		this->normal_indices.push_back(this->normal_vertices.size() - 1);
	}
}

bool Environment::Initialize()
{
	if (this->GLReturnedError("Environment::Initialize - on entry"))
		return false;

    InitWalls();

    InitBar();

	InitBartender();

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

void Environment::InitWalls()
{
    int vertexRows = 100;
	int vertexCols = 100;
	float w_2 = WALLS_WIDTH / 2.0f;
    vec3 color(0.3141f, 0.4626f, 0.0628f);

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
}

void Environment::InitBar()
{
    int vertexRows = 10;
    int vertexCols = 10;
    int slices = 20;
    int stacks = 5;
	float w_2 = BAR_WIDTH / 2.0f;
	float bar_offset = 30.0f;
	float depth = BAR_DEPTH + bar_offset;
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
    
    //beer can 1
    defineDisk(this->vertices, this->vertex_indices, topOfCan + canMove1, disk_up_n, RIGHT, CAN_RADIUS, slices, can_color);
	defineCylinder(this->vertices, this->vertex_indices, topOfCan + canMove1, UP, RIGHT, CAN_RADIUS, CAN_RADIUS, CAN_HEIGHT, slices, stacks, can_color);
	defineDisk(this->vertices, this->vertex_indices, topOfCan - (UP * CAN_HEIGHT) + canMove1, -disk_up_n, RIGHT, CAN_RADIUS, slices, can_color);
    //beer can 2
    defineDisk(this->vertices, this->vertex_indices, topOfCan + canMove2, disk_up_n, RIGHT, CAN_RADIUS, slices, can_color);
	defineCylinder(this->vertices, this->vertex_indices, topOfCan + canMove2, UP, RIGHT, CAN_RADIUS, CAN_RADIUS, CAN_HEIGHT, slices, stacks, can_color);
	defineDisk(this->vertices, this->vertex_indices, topOfCan - (UP * CAN_HEIGHT) + canMove2, -disk_up_n, RIGHT, CAN_RADIUS, slices, can_color);
    //beer can 3
    defineDisk(this->vertices, this->vertex_indices, topOfCan + canMove3, disk_up_n, RIGHT, CAN_RADIUS, slices, can_color);
	defineCylinder(this->vertices, this->vertex_indices, topOfCan + canMove3, UP, RIGHT, CAN_RADIUS, CAN_RADIUS, CAN_HEIGHT, slices, stacks, can_color);
	defineDisk(this->vertices, this->vertex_indices, topOfCan - (UP * CAN_HEIGHT) + canMove3, -disk_up_n, RIGHT, CAN_RADIUS, slices, can_color);
}

void Environment::InitBartender()
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
    vec3 topOfCan = CENTER + (UP * (BAR_HEIGHT + CAN_HEIGHT));
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

void Environment::InitArm(vec3 center, vec3 up, vec3 right)
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

void Environment::TakeDown()
{
	//this->shader->TakeDown();
	super::TakeDown();
}

void Environment::Draw(const mat4 & projection, mat4 modelview, Shader * shader, const ivec2 & size, const float time)
{
	if (this->GLReturnedError("Environment::Draw - on entry"))
		return;

	//Shader shader;
	//if (!shader.Initialize("phong_shader.vert", "phong_shader.frag"))
	//	return;

	//modelview = rotate(modelview, time * 90.0f, vec3(0.0f, 1.0f, 0.0f));
	mat4 mvp = projection * modelview;
	mat3 nm = inverse(transpose(mat3(modelview)));

	/*this->shaders[this->shader_index]->Use();
	this->shaders[this->shader_index]->CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
	this->shaders[this->shader_index]->CustomSetup();*/
	/*shader.Use();
	shader.CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
	shader.CustomSetup();*/
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
		//this->solid_color.Use();
		//this->solid_color.CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
		/*this->shaders[4]->Use();
		this->shaders[4]->CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
		this->shaders[4]->CustomSetup();*/
		/*shader.Use();
		shader.CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
		shader.CustomSetup();*/
		shader->Use();
		shader->CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
		shader->CustomSetup();

		glBindVertexArray(this->normal_array_handle);
		glDrawElements(GL_LINES , this->normal_indices.size(), GL_UNSIGNED_INT , &this->normal_indices[0]);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	//shader.TakeDown();

	if (this->GLReturnedError("Environment::Draw - on exit"))
		return;
}