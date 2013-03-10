#include <iostream>
#include "stool.h"

using namespace std;
using namespace glm;

//#define DRAW_POINTS (1)

// Units are in inches
const float Stool::LEG_OFFSET = 5.898f;
const float Stool::LEG_WIDTH = 1.606f;
const float Stool::LEG_HEIGHT = 22.167f;

Stool::Stool() : Object()
{
	this->shader_index = 0;
	this->drawNormals = false;
	this->drawPoints = false;
}

void Stool::StepShader()
{
	this->shader_index = ++this->shader_index % this->shaders.size();
}

bool Stool::Initialize()
{
	if (this->GLReturnedError("Stool::Initialize - on entry"))
		return false;

#pragma region Shader code should not be here for better O.O.
	//      Nothing shader related ought to be here for better O.O.

	if (!this->play_shader.Initialize("play_shader.vert", "play_shader.frag"))
		return false;

	if (!this->basic_shader.Initialize("basic_shader.vert", "basic_shader.frag"))
		return false;

	if (!this->colored_shader.Initialize("colored_shader.vert", "colored_shader.frag"))
		return false;

	//if (!this->basic_texture_shader.Initialize("basic_texture_shader.vert", "basic_texture_shader.frag"))
	//        return false;

	if (!this->pattern_shader.Initialize("pattern_shader.vert", "pattern_shader.frag"))
		return false;

	this->shaders.push_back(&this->basic_shader);
	this->shaders.push_back(&this->colored_shader);
	//this->shaders.push_back(&this->basic_texture_shader);
	this->shaders.push_back(&this->pattern_shader);
	this->shaders.push_back(&this->play_shader);
#pragma endregion

	/*
	#pragma region Code to make FreeImage work
	//      The picture too, ought not to be here.

	if (!TextureManager::Inst()->LoadTexture((const char *) "stone.jpg", 0))
	return false;

	//      The current "TextureManager" that comes with FreeImage is quite dumb.
	glTexEnvf(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_REPLACE);
	glTexParameterf(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR);
	#pragma endregion*/


	// Create vertices of stool
	InitLeg(vec3(0.0f, LEG_HEIGHT/2.0f, LEG_OFFSET), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));
	InitLeg(vec3(LEG_OFFSET, LEG_HEIGHT/2.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
	InitLeg(vec3(0.0f, LEG_HEIGHT/2.0f, -LEG_OFFSET), vec3(0.0f, 1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f));
	InitLeg(vec3(-LEG_OFFSET, LEG_HEIGHT/2.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));

	InitDiskSupport(vec3(0.0f, LEG_HEIGHT, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), 2.295f, 2.524f, 0.918f);
	InitDiskSupport(vec3(0.0f, LEG_HEIGHT-4.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), 3.213f, 3.442f, 0.918f);

	//InitRingSupport();


	//      The vertex array serves as a handle for the whole bundle.
	glGenVertexArrays(1, &this->vertex_array_handle);
	glBindVertexArray(this->vertex_array_handle);

	//      The vertex buffer serves as a container for the memory to be defined.
	glGenBuffers(1, &this->vertex_coordinate_handle);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_coordinate_handle);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(VertexAttributes), &this->vertices[0], GL_STATIC_DRAW);

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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributes), (GLvoid *) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributes), (GLvoid *) (sizeof(vec3)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributes), (GLvoid *) (sizeof(vec3) * 2));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttributes), (GLvoid *) (sizeof(vec3) * 3));

	// Each of the attributes to be used must be enabled.
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

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
	defineRhombus(this->vertices, this->vertex_indices, vec3(center - w_2*right_n + h_2*up_n + (-h_2*tanf(angle) + w_2)*norm_n), tiltedUp, right_n, LEG_HEIGHT/sin(PI/2 - angle), LEG_WIDTH, 0.0f, vertexRows, vertexCols);
	defineRhombus(this->vertices, this->vertex_indices, vec3(center + w_2*right_n + h_2*up_n + (-h_2*tanf(angle) + w_2)*norm_n), up_n, rotRight, LEG_HEIGHT, LEG_WIDTH, -angle, vertexRows, vertexCols);
	defineRhombus(this->vertices, this->vertex_indices, vec3(center + w_2*right_n + h_2*up_n + (-h_2*tanf(angle) - w_2)*norm_n), tiltedUp, -right_n, LEG_HEIGHT/sin(PI/2 - angle), LEG_WIDTH, 0.0f, vertexRows, vertexCols);
	defineRhombus(this->vertices, this->vertex_indices, vec3(center - w_2*right_n + h_2*up_n + (-h_2*tanf(angle) - w_2)*norm_n), up_n, -rotRight, LEG_HEIGHT, LEG_WIDTH, angle, vertexRows, vertexCols);
	// Top and Bottom
	defineRhombus(this->vertices, this->vertex_indices, vec3(center - w_2*right_n + h_2*up_n + (-h_2*tanf(angle) + w_2)*norm_n), -right_n, rotRight, LEG_WIDTH, LEG_WIDTH, 0.0f, vertexCols, vertexCols);
	defineRhombus(this->vertices, this->vertex_indices, vec3(center - w_2*right_n - h_2*up_n + (h_2*tanf(angle) + w_2)*norm_n), -rotRight, right_n, LEG_WIDTH, LEG_WIDTH, 0.0f, vertexCols, vertexCols);


	// We will have to see if the shading works correctly. The vertices on the edges of these rhombi will overlap. So there will essentially be two normals on the corners.
}

void Stool::InitDiskSupport(vec3 center, vec3 up, vec3 right, float radiusTop, float radiusBot, float height)
{
	int slices = 20;
	int stacks = 2;

	defineCylinder(this->vertices, this->vertex_indices, center, up, right, radiusTop, radiusBot, height, slices, stacks);
}

void Stool::InitRingSupport()
{
	vec3 center(0.0f, 0.0f, 0.0f);
	float outerRadius = 1.0f;

	defineRing(this->vertices, this->vertex_indices);
}

void Stool::TakeDown()
{
	super::TakeDown();
}

void Stool::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size, const float time)
{
	if (this->GLReturnedError("Stool::Draw - on entry"))
		return;

	//modelview = rotate(modelview, time * 90.0f, vec3(0.0f, 1.0f, 0.0f));
	mat4 mvp = projection * modelview;

	this->shaders[this->shader_index]->Use();
	//TextureManager::Inst()->BindTexture(0, 0);
	this->shaders[this->shader_index]->CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(transpose(inverse(modelview))));
	this->shaders[this->shader_index]->CustomSetup();

	glBindVertexArray(this->vertex_array_handle);

	if (this->drawPoints)
		glDrawArrays(GL_POINTS, 0, this->vertex_indices.size());
	else
		glDrawElements(GL_TRIANGLES, this->vertex_indices.size(), GL_UNSIGNED_INT , &this->vertex_indices[0]);

	glUseProgram(0);
	glBindVertexArray(0);

	/*if (this->drawNormals)
	{
		this->solid_color.Use();
		this->solid_color.CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
		glBindVertexArray(this->normal_array_handle);
		glDrawElements(GL_LINES , this->normal_indices.size(), GL_UNSIGNED_INT , &this->normal_indices[0]);
		glBindVertexArray(0);
		glUseProgram(0);
	}*/

	if (this->GLReturnedError("Stool::Draw - on exit"))
		return;
}