/*
Perry Kivolowitz - University of Wisconsin - Madison 
Computer Sciences Department

A sample hello world like program demonstrating modern
OpenGL techniques. 

Object() is a sample base class upon which drawable objects might
be derived. It assumes that all drawable objects have some geometry
to buffer.

Created:        2/25/13
*/

#include <iostream>
#include <assert.h>
#include "object.h"

using namespace std;

/*
Notice the destructor in this case asserts that all resources
that don't go away by themselves have ALREADY been released. This
is because the destructor might be called after a GL context has
been destroyed, so I force the user of this class to call the
TakeDown() purposefully.
*/

Object::Object()
{
	this->InternalInitialize();
}

Object::~Object()
{
	assert(this->vertex_array_handle == GLuint(-1));
	assert(this->vertex_coordinate_handle == GLuint(-1));
}

void Object::TakeDown()
{
	if (this->vertex_array_handle != GLuint(-1))
		glDeleteVertexArrays(1, &this->vertex_array_handle);

	if (this->vertex_coordinate_handle != GLuint(-1))
		glDeleteBuffers(1, &this->vertex_coordinate_handle);

	this->InternalInitialize();
}

bool Object::Initialize()
{
	this->InternalInitialize();
	return true;
}

void Object::InternalInitialize()
{
	this->vertex_array_handle = this->vertex_coordinate_handle = GLuint(-1);
}

bool Object::GLReturnedError(char * s)
{
	bool return_error = false;
#ifdef _DEBUG
	GLenum glerror;

	while ((glerror = glGetError()) != GL_NO_ERROR)
	{
		return_error = true;
		cerr << s << ": " << gluErrorString(glerror) << endl;
	}
#endif
	return return_error;
}