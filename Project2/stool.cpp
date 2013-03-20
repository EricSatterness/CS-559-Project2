#include <iostream>
#include "stool.h"

using namespace std;
using namespace glm;

//#define DRAW_POINTS (1)

const vec3 Stool::CENTER(0.0f, 0.0f, 0.0f);
const vec3 Stool::UP(0.0f, 1.0f, 0.0f);
const vec3 Stool::RIGHT(1.0f, 0.0f, 0.0f);

// Units are in inches
const float Stool::LEG_OFFSET = 5.898f;
const float Stool::LEG_WIDTH = 1.606f;
const float Stool::LEG_HEIGHT = 22.167f;
const float Stool::SEAT_RADIUS_BOT = 3.442f;
const float Stool::SEAT_RADIUS_TOP = 5.875f;
const float Stool::SEAT_THICKNESS = 0.810f;
const float Stool::SEAT_OFFSET = 0.8f;
const float Stool::ROD_RADIUS = 0.521f;
const float Stool::ROD_HEIGHT = 10.694f;
const float Stool::RING_RADIUS_INNER = 0.298f;
const float Stool::RING_RADIUS_OUTER = 5.952f;
const float Stool::RING_OFFSET = 6.908f;

Stool::Stool() : Object()
{
	//this->shader_index = 0;
	this->draw_normals = false;
	this->draw_points = false;
}

// After trying nearly every possible way of storing some sort of reference to the shader I realized that you can simply pass the shader pointer in the object's draw method... you don't need to store the shader in the object at all. Hurray!
//void Stool::StepShader()
//{
//	this->shader_index = ++this->shader_index % this->shaders.size();
//}

//bool Stool::SetShader(char * vertex_shader_file, char * fragment_shader_file)
//{
//	this->shader->TakeDown();
//	delete this->shader;
//	//this->shader.TakeDown();
//	//delete this->shader;
//
//	//Shader * newShader = new Shader();
//	//if (!this->shader.Initialize(vertex_shader_file, fragment_shader_file))
//	//	return false;
//	//if (!newShader->Initialize(vertex_shader_file, fragment_shader_file))
//	//	return false;
//	//this->shader = newShader;
//
//	return true;
//}

void Stool::BuildNormalVisualizationGeometry()
{
	/*float normal_scalar = 1.0f;
	for (int i = 0; i < int(this->vertices.size()); i++)
	{
		this->normal_vertices.push_back(VertexAttributesP(this->vertices[i].position));
		this->normal_vertices.push_back(VertexAttributesP(this->vertices[i].position + this->vertices[i].normal * normal_scalar));
		this->normal_indices.push_back(this->normal_vertices.size() - 2);
		this->normal_indices.push_back(this->normal_vertices.size() - 1);
	}*/

	super::BuildNormalVisualizationGeometry();
}

//bool Stool::Initialize(char * vertex_shader_file, char * fragment_shader_file)
bool Stool::Initialize()
{
	if (this->GLReturnedError("Stool::Initialize - on entry"))
		return false;

	/*Shader * newShader = new Shader();
	if (!newShader->Initialize(vertex_shader_file, fragment_shader_file))
		return false;
	this->shader = newShader;*/

#pragma region Shader code should not be here for better O.O.
	//Nothing shader related ought to be here for better O.O.

	/*if (!this->phong_shader.Initialize("phong_shader.vert", "phong_shader.frag"))
		return false;

	if (!this->gouraud_shader.Initialize("gouraud_shader.vert", "gouraud_shader.frag"))
		return false;

	if (!this->flat_shader.Initialize("flat_shader.vert", "flat_shader.frag"))
		return false;

	if (!this->colored_shader.Initialize("colored_shader.vert", "colored_shader.frag"))
		return false;

	if (!this->basic_shader.Initialize("basic_shader.vert", "basic_shader.frag"))
		return false;

	this->shaders.push_back(&this->phong_shader);
	this->shaders.push_back(&this->gouraud_shader);
	this->shaders.push_back(&this->flat_shader);
	this->shaders.push_back(&this->colored_shader);
	this->shaders.push_back(&this->basic_shader);*/
#pragma endregion

	// Create vertices of stool
	InitLeg(vec3(0.0f, LEG_HEIGHT/2.0f, LEG_OFFSET), UP, vec3(1.0f, 0.0f, 0.0f));
	InitLeg(vec3(LEG_OFFSET, LEG_HEIGHT/2.0f, 0.0f), UP, vec3(0.0f, 0.0f, -1.0f));
	InitLeg(vec3(0.0f, LEG_HEIGHT/2.0f, -LEG_OFFSET), UP, vec3(-1.0f, 0.0f, 0.0f));
	InitLeg(vec3(-LEG_OFFSET, LEG_HEIGHT/2.0f, 0.0f), UP, vec3(0.0f, 0.0f, 1.0f));

	InitDiskSupport(CENTER + (UP * LEG_HEIGHT), UP, RIGHT, 2.295f, 2.524f, 0.918f);
	InitDiskSupport(CENTER + (UP * (LEG_HEIGHT - 4.0f)), UP, RIGHT, 3.213f, 3.442f, 0.918f);
	
	InitRingSupport();

	InitSeat();

	InitSeatRod();

	//InitDiskSupport(CENTER, UP, RIGHT, 8.0f, 8.0f, 10.0f);

	this->BuildNormalVisualizationGeometry();


	////The vertex array serves as a handle for the whole bundle.
	//glGenVertexArrays(1, &this->vertex_array_handle);
	//glBindVertexArray(this->vertex_array_handle);

	////The vertex buffer serves as a container for the memory to be defined.
	//glGenBuffers(1, &this->vertex_coordinate_handle);
	//glBindBuffer(GL_ARRAY_BUFFER, this->vertex_coordinate_handle);
	//glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(VertexAttributesPCNT), &this->vertices[0], GL_STATIC_DRAW);
	if (!this->PostGLInitialize(&this->vertex_array_handle, &this->vertex_coordinate_handle, this->vertices.size() * sizeof(VertexAttributesPCNT), &this->vertices[0]))
		return false;

	/*
	This is an example putting all vertex attributes in a single block of member and using 
	offsets and strides to thread through a single "column" of data. In this way only a 
	single block of memory is used with the hoped for advantage of memory coherency as 
	well as the real advantage of fewer function calls.

	0 .. n refers to the locations at which the shaders will pick up the various attributes. 
	Each attribute's type (float) and length (3 and 2) are specified along with the offset
	at which the particular attribute is found. Finally, the stride over which to cross to
	find the next instance of the attribute (sizeof()) is specified.
	*/

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

	if (this->GLReturnedError("Stool::Initialize - on exit"))
		return false;

	return true;
}

void Stool::InitLeg(vec3 center, vec3 up, vec3 right)
{
	//int vertexRows = 111;
	//int vertexCols = 8;
	int vertexRows = 10;
	int vertexCols = 2;
	vec3 color(0.8706f, 0.7126f, 0.5294f);

	vec3 up_n = glm::normalize(up);
	vec3 right_n = glm::normalize(right);
	vec3 norm_n = glm::normalize(cross(right_n, up_n));
	float w_2 = LEG_WIDTH / 2.0f;
	float h_2 = LEG_HEIGHT / 2.0f;
	float angle = 15.0f*PI/180.0f;

	// Single rhombus
	//defineRhombus(this->vertices, this->vertex_indices, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), LEG_HEIGHT, LEG_WIDTH, angle, vertexRows, vertexCols);

	// Rectangle based from center
	//defineRhombus(this->vertices, this->vertex_indices, vec3(center.x - w_2, center.y + h_2, center.z + w_2), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), LEG_HEIGHT, LEG_WIDTH, 0.0f);
	//defineRhombus(this->vertices, this->vertex_indices, vec3(center.x + w_2, center.y + h_2, center.z + w_2), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), LEG_HEIGHT, LEG_WIDTH, 0.0f);
	//defineRhombus(this->vertices, this->vertex_indices, vec3(center.x + w_2, center.y + h_2, center.z - w_2), vec3(0.0f, 1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), LEG_HEIGHT, LEG_WIDTH, 0.0f);
	//defineRhombus(this->vertices, this->vertex_indices, vec3(center.x - w_2, center.y + h_2, center.z - w_2), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), LEG_HEIGHT, LEG_WIDTH, 0.0f);

	// This draws the leg correctly but does not take any other rotations into account
	/*// Tilted rectangle
	defineRhombus(this->vertices, this->vertex_indices, vec3((center.x - w_2), (center.y + h_2), (center.z - h_2*tanf(angle) + w_2)), vec3(0.0f, cosf(angle), sinf(-angle)), vec3(1.0f, 0.0f, 0.0f), LEG_HEIGHT/sin(PI/2 - angle), LEG_WIDTH, 0.0f, vertexRows, vertexCols);
	defineRhombus(this->vertices, this->vertex_indices, vec3((center.x + w_2), (center.y + h_2), (center.z - h_2*tanf(angle) + w_2)), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), LEG_HEIGHT, LEG_WIDTH, -angle, vertexRows, vertexCols);
	defineRhombus(this->vertices, this->vertex_indices, vec3((center.x + w_2), (center.y + h_2), (center.z - h_2*tanf(angle) - w_2)), vec3(0.0f, cosf(angle), sinf(-angle)), vec3(-1.0f, 0.0f, 0.0f), LEG_HEIGHT/sin(PI/2 - angle), LEG_WIDTH, 0.0f, vertexRows, vertexCols);
	defineRhombus(this->vertices, this->vertex_indices, vec3((center.x - w_2), (center.y + h_2), (center.z - h_2*tanf(angle) - w_2)), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), LEG_HEIGHT, LEG_WIDTH, angle, vertexRows, vertexCols);
	// Top and Bottom
	defineRhombus(this->vertices, this->vertex_indices, vec3((center.x - w_2), (center.y + h_2), (center.z - h_2*tanf(angle) + w_2)), vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), LEG_WIDTH, LEG_WIDTH, 0.0f, vertexCols, vertexCols);
	defineRhombus(this->vertices, this->vertex_indices, vec3((center.x - w_2), (center.y - h_2), (center.z + h_2*tanf(angle) + w_2)), vec3(0.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), LEG_WIDTH, LEG_WIDTH, 0.0f, vertexCols, vertexCols);*/


	// In order to specify the up and right vectors for each rhombus we need be able to rotate the specified up and right vetors.
	// Rotate the up vector by the leg angle to get the up vector of the slanted faces
	// Rotate the right vector by PI/2 to get the right vector of the sheared faces
	mat3 rMatAroundRight = createRotationMatrix(right_n, angle);
	mat3 rMatAroundUp = createRotationMatrix(up_n, -PI/2.0f);
	vec3 tiltedUp = glm::normalize(rMatAroundRight * up_n);
	vec3 rotRight = glm::normalize(rMatAroundUp * right_n);

	// Draw the stool leg while taking into consideration both the center, up, and right vectors
	// Tilted rectangles
	defineRhombus(this->vertices, this->vertex_indices, vec3(center - w_2*right_n + h_2*up_n + (-h_2*tanf(angle) + w_2)*norm_n), tiltedUp, right_n, LEG_HEIGHT/sin(PI/2 - angle), LEG_WIDTH, 0.0f, vertexRows, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(center + w_2*right_n + h_2*up_n + (-h_2*tanf(angle) + w_2)*norm_n), up_n, rotRight, LEG_HEIGHT, LEG_WIDTH, -angle, vertexRows, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(center + w_2*right_n + h_2*up_n + (-h_2*tanf(angle) - w_2)*norm_n), tiltedUp, -right_n, LEG_HEIGHT/sin(PI/2 - angle), LEG_WIDTH, 0.0f, vertexRows, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(center - w_2*right_n + h_2*up_n + (-h_2*tanf(angle) - w_2)*norm_n), up_n, -rotRight, LEG_HEIGHT, LEG_WIDTH, angle, vertexRows, vertexCols, color);
	// Top and Bottom
	defineRhombus(this->vertices, this->vertex_indices, vec3(center - w_2*right_n + h_2*up_n + (-h_2*tanf(angle) + w_2)*norm_n), -right_n, rotRight, LEG_WIDTH, LEG_WIDTH, 0.0f, vertexCols, vertexCols, color);
	defineRhombus(this->vertices, this->vertex_indices, vec3(center - w_2*right_n - h_2*up_n + (h_2*tanf(angle) + w_2)*norm_n), -rotRight, right_n, LEG_WIDTH, LEG_WIDTH, 0.0f, vertexCols, vertexCols, color);


	// We will have to see if the shading works correctly. The vertices on the edges of these rhombi will overlap. So there will essentially be two normals on the corners.
}

void Stool::InitDiskSupport(vec3 center, vec3 up, vec3 right, float radiusTop, float radiusBot, float height)
{
	int slices = 30;
	int stacks = 2;
	vec3 color(0.0f, 0.5f, 0.0f);

	vec3 right_n = glm::normalize(right);
	vec3 cylinder_up_n = glm::normalize(up);
	vec3 disk_up_n = -glm::normalize(cross(right_n, cylinder_up_n));
	float h_2 = height / 2.0f;

	// Building from top down
	defineDisk(this->vertices, this->vertex_indices, center, disk_up_n, right_n, radiusTop, slices, color);
	defineCylinder(this->vertices, this->vertex_indices, center, cylinder_up_n, right_n, radiusTop, radiusBot, height, slices, stacks, color);
	defineDisk(this->vertices, this->vertex_indices, center - (cylinder_up_n * height), -disk_up_n, right_n, radiusBot, slices, color);
}

void Stool::InitRingSupport()
{
	int slices = 30;
	int stacks = 8;
	vec3 color(0.5f, 0.0f, 0.0f);

	defineRing(this->vertices, this->vertex_indices, CENTER + (UP * RING_OFFSET), UP, RIGHT, RING_RADIUS_INNER, RING_RADIUS_OUTER, slices, stacks, color);
}

void Stool::InitSeat()
{
	int slices = 30;
	int stacks = 1;
	vec3 color(0.0f, 0.0f, 0.5f);

	float thickness_2 = SEAT_THICKNESS / 2.0f;
	vec3 topOfSeat = CENTER + (UP * (LEG_HEIGHT + SEAT_THICKNESS + SEAT_OFFSET));
	vec3 disk_up_n = -glm::normalize(cross(RIGHT, UP));

	// Building from top down
	defineDisk(this->vertices, this->vertex_indices, topOfSeat, disk_up_n, RIGHT, SEAT_RADIUS_TOP, slices, color);
	defineCylinder(this->vertices, this->vertex_indices, topOfSeat, UP, RIGHT, SEAT_RADIUS_TOP, SEAT_RADIUS_TOP, thickness_2, slices, stacks, color);
	defineCylinder(this->vertices, this->vertex_indices, topOfSeat - (UP * thickness_2), UP, RIGHT, SEAT_RADIUS_TOP, SEAT_RADIUS_BOT, thickness_2, slices, stacks, color);
	defineDisk(this->vertices, this->vertex_indices, topOfSeat - (UP * SEAT_THICKNESS), -disk_up_n, RIGHT, SEAT_RADIUS_BOT, slices, color);
}

void Stool::InitSeatRod()
{
	int slices = 10;
	int stacks = 5;
	vec3 color(0.3f, 0.1f, 0.5f);

	vec3 topOfRod = CENTER + (UP * (LEG_HEIGHT + SEAT_OFFSET));
	vec3 disk_up_n = -glm::normalize(cross(RIGHT, UP));

	defineDisk(this->vertices, this->vertex_indices, topOfRod, disk_up_n, RIGHT, ROD_RADIUS, slices, color);
	defineCylinder(this->vertices, this->vertex_indices, topOfRod, UP, RIGHT, ROD_RADIUS, ROD_RADIUS, ROD_HEIGHT, slices, stacks, color);
	defineDisk(this->vertices, this->vertex_indices, topOfRod - (UP * ROD_HEIGHT), -disk_up_n, RIGHT, ROD_RADIUS, slices, color);
}

void Stool::TakeDown()
{
	//this->shader->TakeDown();
	super::TakeDown();
}

void Stool::Draw(const mat4 & projection, mat4 modelview, Shader * shader, const ivec2 & size, const float time)
{
	if (this->GLReturnedError("Stool::Draw - on entry"))
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

	if (this->GLReturnedError("Stool::Draw - on exit"))
		return;
}
