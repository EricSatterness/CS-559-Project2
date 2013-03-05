#include <iostream>
#include "stool.h"

using namespace std;
using namespace glm;

// Assign values to static constants
const float Stool::SCALE_FACTOR = 2.2949f;

#define DRAW_POINTS (1)
#define DRAW_LINES (0)

Stool::Stool() : Object()
{
	this->shader_index = 0;
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
	InitLeg();

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

void Stool::InitLeg()
{
	//float width = 1.606f;
	//float height = 22.167f;
	//int vertexRows = 111;
	//int vertexCols = 8;

	float width = 2.0f;
	float height = 20.0f;
	int vertexRows = 20;
	int vertexCols = 2;

	vec3 center(0.0f, 0.0f, 0.0f);
	float w_2 = width / 2.0f;
	float h_2 = height / 2.0f;
	float angle = 15.0f*PI/180.0f;


	// Cube
	//DefineRhombus(this->vertices, this->vertex_indices, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), height, width, angle, vertexRows, vertexCols);
	//DefineRhombus(this->vertices, this->vertex_indices, vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), height, width, 0.0f);
	//DefineRhombus(this->vertices, this->vertex_indices, vec3(1.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), height, width, 0.0f);
	//DefineRhombus(this->vertices, this->vertex_indices, vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), height, width, 0.0f);

	// Rectangle based from center
	//DefineRhombus(this->vertices, this->vertex_indices, vec3(center.x - w_2, center.y + h_2, center.z + w_2), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), height, width, 0.0f);
	//DefineRhombus(this->vertices, this->vertex_indices, vec3(center.x + w_2, center.y + h_2, center.z + w_2), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), height, width, 0.0f);
	//DefineRhombus(this->vertices, this->vertex_indices, vec3(center.x + w_2, center.y + h_2, center.z - w_2), vec3(0.0f, 1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), height, width, 0.0f);
	//DefineRhombus(this->vertices, this->vertex_indices, vec3(center.x - w_2, center.y + h_2, center.z - w_2), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), height, width, 0.0f);

	// Tilted rectangle
	//DefineRhombus(this->vertices, this->vertex_indices, vec3(center.x - w_2, (center.y*cosf(angle) + h_2), (center.z*sinf(angle) + w_2)), vec3(0.0f, cosf(angle), sinf(-angle)), vec3(1.0f, 0.0f, 0.0f), height, width, 0.0f, vertexRows, vertexCols);
	DefineRhombus(this->vertices, this->vertex_indices, vec3(center.x + w_2, (center.y*cosf(angle) + h_2), (center.z*sinf(angle) + w_2)), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), height*sin(PI/2 - angle), width, -angle, vertexRows, vertexCols);
	//DefineRhombus(this->vertices, this->vertex_indices, vec3(center.x + w_2, (center.y*cosf(angle) + h_2), (center.z*sinf(angle) - w_2)), vec3(0.0f, cosf(angle), sinf(-angle)), vec3(-1.0f, 0.0f, 0.0f), height, width, 0.0f, vertexRows, vertexCols);
	//DefineRhombus(this->vertices, this->vertex_indices, vec3(center.x - w_2, (center.y*cosf(angle) + h_2), (center.z*sinf(angle) - w_2)), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), height*sin(PI/2 - angle), width, angle, vertexRows, vertexCols);

	// Top and Bottom
	//DefineRhombus(this->vertices, this->vertex_indices, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), height, width, 0.0f);
	//DefineRhombus(this->vertices, this->vertex_indices, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), height, width, 0.0f);
}

void Stool::TakeDown()
{
	super::TakeDown();
}

void Stool::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size, const float time)
{
	// Draw a cube
	//glColor3d(0.8706, 0.7126, 0.5294);
	//glLoadMatrixf(value_ptr(modelview));
	//glutSolidCube(1);


	if (this->GLReturnedError("Stool::Draw - on entry"))
		return;

	//modelview = rotate(modelview, time * 90.0f, vec3(0.0f, 1.0f, 0.0f));
	//modelview = scale(modelview, vec3(SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR));
	//modelview = scale(modelview, vec3(10.0f, 10.0f, 10.0f));
	mat4 mvp = projection * modelview;

	this->shaders[this->shader_index]->Use();
	//TextureManager::Inst()->BindTexture(0, 0);
	this->shaders[this->shader_index]->CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(transpose(inverse(modelview))));
	this->shaders[this->shader_index]->CustomSetup();

	glBindVertexArray(this->vertex_array_handle);
#if (DRAW_POINTS)
	glDrawArrays(GL_POINTS, 0, this->vertex_indices.size());
#elif (DRAW_LINES)
	glDrawElements(GL_LINES, this->vertex_indices.size(), GL_UNSIGNED_INT , &this->vertex_indices[0]);
#else
	glDrawElements(GL_TRIANGLES, this->vertex_indices.size(), GL_UNSIGNED_INT , &this->vertex_indices[0]);
#endif

	glUseProgram(0);
	glBindVertexArray(0);

	if (this->GLReturnedError("Stool::Draw - on exit"))
		return;
}