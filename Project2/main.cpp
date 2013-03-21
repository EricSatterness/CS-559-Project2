#include <iostream>
#include <assert.h>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stool.h"
#include "axes.h"
#include "grid.h"
#include "environment.h"
#include "person.h"
#include "bar.h"
#include "can.h"
#include "walls.h"

// Useful links
//http://www.arcsynthesis.org/gltut/index.html

using namespace std;
using namespace glm;

//#define DEBUG 1

#pragma region Data Structs
// Took this out because it is easier to just pass a shader pointer to the draw method of the objects
// Holds the filenames of a vertex shader and fragement shader
//struct ShaderFiles
//{
//	char * vert;
//	char * frag;
//} phong_shader, gouraud_shader, flat_shader, colored_shader, basic_shader;

// Keeps track of data relevant to each individual window
struct WindowData
{
	int window_handle;
	ivec2 size;
	float window_aspect;
	mat4 projection_matrix, modelview_matrix;
	bool wireframe, normals, points, debug_mode, axes, grid;
	vector<string> instructions;
	int shader_index;
	//std::vector<ShaderFiles *> shaders;
	std::vector<Shader *> shaders;
	std::vector<Object *> objects;
	int debug_index;
	std::vector<Object *> debug_objects;
} window;

// Keeps track of the 1st person camera data
struct CameraData
{
	/*float rotX, rotY, rotZ;
	float tranX, tranZ, tranY;
	float lens, zoom;
	float spinXZ, spinY;
	float rad;*/
	float rotX, rotY;
	float tranX, tranY, tranZ;
	float zoom;
	// Freemode changes the rotation to be about the camera's position rather than the camera's target
	bool freemode;
	vec3 position, direction, up, right;
} mainCamera;
#pragma endregion

#pragma region Constants
// These have been removed because the axes and grid are now objects
// These represent the axes
//const float	ORG[3] = {0,0,0},
//			XP[3] = {5,0,0},
//			YP[3] = {0,5,0},
//			ZP[3] = {0,0,5};
//// Used for constructing the floor grid. Units are in feet
//const float GRIDWIDTH = 10.0;
#pragma endregion

#pragma region Variables
// Move speed is in feet
float moveSpeed = 0.1f;
// Rotate speed is in degrees
float rotateSpeed = 1.0f;
// Zoom speed modifies the camera fov in units of feet
float zoomSpeed = 0.5f;

// Shaders
Shader phong_shader, gouraud_shader, flat_shader, colored_shader, basic_shader, brick_shader, phong_attenuated_shader;

// Objects to be drawn in the scene
Axes *axes;
Grid *grid;
Stool *stool1;
Stool *stool2;
Stool *stool3;
Person *person;
Bar *bar;
Can *can1;
Can *can2;
Can *can3;
Walls *walls;
#pragma endregion

// Reset the camera variables when switching between camera modes.
// This is easier than trying to switch modes at the camera's current position because both modes use the rotation variables and they use them differently.
void ResetCamera()
{
	mainCamera.rotX = 0.0;
	mainCamera.rotY = 0.0;
	mainCamera.tranX = 0.0;
	mainCamera.tranZ = 0.0;
	mainCamera.zoom = 40.0;
	mainCamera.position = vec3(0.0f, 0.0f, 15.0f);
	mainCamera.direction = vec3(0.0f, 0.0f, -1.0f);
	mainCamera.up = vec3(0.0f, 1.0f, 0.0f);
	mainCamera.right = vec3(0.0f, 0.0f, 0.0f);
}

// Take care of taking down and deleting any items
void CloseFunc()
{
	window.window_handle = -1;

	if (axes != NULL)
	{
		axes->TakeDown();
		delete axes;
	}
	if (grid != NULL)
	{
		grid->TakeDown();
		delete grid;
	}
	if (walls != NULL)
	{
		walls->TakeDown();
		delete walls;
	}

	for (int i = 0; i < (int)window.objects.size(); i++)
	{
		if (window.objects[i] != NULL)
		{
			window.objects[i]->TakeDown();
			delete window.objects[i];
		}

	}
}

void ReshapeFunc(int w, int h)
{
	if (window.window_handle != -1 &&  h > 0)
	{
		window.size = ivec2(w, h);
		window.window_aspect = float(w) / float(h);
	}
}

void KeyboardFunc(unsigned char c, int x, int y)
{
	if (window.window_handle == -1)
		return;

	switch (c)
	{
	// Started modifying the code for the camera rotation but never got a chance to finish. So decided to just comment it out and go back to what I had before
	//// IJKL strafe the camera
	//case 'i':
	//	mainCamera.tranZ -= 1 * moveSpeed;
	//	mainCamera.rotZ -= 1 * moveSpeed;
	//	break;
	//case 'k':
	//	mainCamera.tranZ += 1 * moveSpeed;
	//	mainCamera.rotZ += 1 * moveSpeed;
	//	break;
	//case 'j':
	//	mainCamera.tranX -= 1 * moveSpeed;
	//	mainCamera.rotX -= 1 * moveSpeed;
	//	break;
	//case 'l':
	//	mainCamera.tranX += 1 * moveSpeed;
	//	mainCamera.rotX += 1 * moveSpeed;
	//	break;

	//// Zoom camera in and out
	//case '=':
	//	mainCamera.lens -= 1 * zoomSpeed;
	//	break;
	//case '-':
	//	mainCamera.lens += 1 * zoomSpeed;
	//	break;


	// IKJL strafe the camera forward, backward, left, or right
	case 'i':
		mainCamera.tranZ -= 1 * moveSpeed;
		mainCamera.position += mainCamera.direction;
		break;
	case 'k':
		mainCamera.tranZ += 1 * moveSpeed;
		mainCamera.position -= mainCamera.direction;
		break;
	case 'j':
		mainCamera.tranX -= 1 * moveSpeed;
		mainCamera.position -= cross(mainCamera.direction, mainCamera.up);
		break;
	case 'l':
		mainCamera.tranX += 1 * moveSpeed;
		mainCamera.position += cross(mainCamera.direction, mainCamera.up);
		break;

	// Move the camera up or down
	case 'u':
		mainCamera.tranY += 1 * moveSpeed;
		mainCamera.position += mainCamera.up;
		break;
	case 'o':
		mainCamera.tranY -= 1 * moveSpeed;
		mainCamera.position -= mainCamera.up;
		break;

	// Zoom camera in and out
	case '=':
		mainCamera.zoom -= 1 * zoomSpeed;
		break;
	case '-':
		mainCamera.zoom += 1 * zoomSpeed;
		break;

	// Toggle camera between free and rotate mode
	case 'c':
		mainCamera.freemode = !mainCamera.freemode;
		ResetCamera();
		break;

	// Step through shaders
	case 's':
		window.shader_index = ++window.shader_index % window.shaders.size();
		//stool1->SetShader(window.shaders[window.shader_index]->frag, window.shaders[window.shader_index]->vert);
		//stool1->StepShader();
		break;

	// Turn on/off wireframe, normals, points, axes, and grid
	case 'w':
		window.wireframe = !window.wireframe;
		break;
	case 'n':
		//stool1->EnableNormals(window.normals = !window.normals);
		window.normals = !window.normals;
		for (int i = 0; i < (int)window.objects.size(); i++)
		{
			window.objects[i]->EnableNormals(window.normals);
		}
		break;
	case 'p':
		//stool1->EnablePoints(window.points = !window.points);
		window.points = !window.points;
		for (int i = 0; i < (int)window.objects.size(); i++)
		{
			window.objects[i]->EnablePoints(window.points);
		}
		break;
	case 'a':
		window.axes = !window.axes;
		break;
	case 'g':
		window.grid = !window.grid;
		break;

	case 'x':
	case 27:
		glutLeaveMainLoop();
		return;
	}

	// Ensure camera can't strafe indefinitely
	if (mainCamera.tranZ > 15) mainCamera.tranZ = 15;
	if (mainCamera.tranZ < -15) mainCamera.tranZ = -15;
	if (mainCamera.tranY > 15) mainCamera.tranY = 15;
	if (mainCamera.tranY < -15) mainCamera.tranY = -15;
	if (mainCamera.tranX > 15) mainCamera.tranX = 15;
	if (mainCamera.tranX < -15) mainCamera.tranX = -15;
	/*clamp(mainCamera.tranX, -20.0f, 20.0f);
	clamp(mainCamera.tranY, -20.0f, 20.0f);
	clamp(mainCamera.tranZ, -20.0f, 20.0f);*/

	// Ensure camera can't zoom indefinitely
	if (mainCamera.zoom > 60) mainCamera.zoom = 60;
	if (mainCamera.zoom < 10) mainCamera.zoom = 10;

	// I can't seem to get clamp to work... not sure why this is but it doesn't do anything even on a simple int...
	//clamp(mainCamera.zoom, 10.0f, 60.0f);
	//float derp = 65.0f;
	//int herp = 5;
	//if (mainCamera.zoom > 60)
	//	bool derp = true;
	//if (mainCamera.zoom < 10)
	//	bool derp = true;
	//clamp(mainCamera.zoom, 10.0f, 60.0f);
	//clamp(derp, 10.0f, 60.0f);
	//clamp(herp, 10, 60);
}

void SpecialFunc(int key, int x, int y)
{
	// Started modifying the code for the camera movement but never got a chance to finish. So decided to just comment it out and go back to what I had before
	//switch(key)
	//{
	//// Rotate the 1st person camera using arrow keys
	//case GLUT_KEY_LEFT:
	//	mainCamera.spinXZ -= 1.0f;
	//	break;
	//case GLUT_KEY_RIGHT:
	//	mainCamera.spinXZ += 1.0f;
	//	break;
	//case GLUT_KEY_UP:
	//	mainCamera.spinY -= 1.0f;
	//	if (mainCamera.spinY <= -90)
	//		mainCamera.spinY = -89;
	//	break;
	//case GLUT_KEY_DOWN:
	//	mainCamera.spinY += 1.0f;
	//	if (mainCamera.rotY >= 90)
	//		mainCamera.rotY = 89;
	//	break;

	//// Enable/diable debug mode
	//case GLUT_KEY_F1:
	//	window.debug_mode = !window.debug_mode;
	//	break;

	//case GLUT_KEY_PAGE_UP:
	//	mainCamera.tranY += 1 * moveSpeed;
	//	mainCamera.rotY += 1 * moveSpeed;
	//	break;

	//case GLUT_KEY_PAGE_DOWN:
	//	mainCamera.tranY -= 1 * moveSpeed;
	//	mainCamera.rotY -= 1 * moveSpeed;
	//	break;

	//case GLUT_KEY_HOME:
	//	mainCamera.zoom -= 1.0f;
	//	break;

	//case GLUT_KEY_END:
	//	mainCamera.zoom += 1.0f;
	//	break;
	//}

	//mainCamera.rad = 0.01745f;
	//mainCamera.rotY = mainCamera.zoom * cos(mainCamera.rad * mainCamera.spinY);
	//mainCamera.rotX = mainCamera.zoom * sin(mainCamera.rad * mainCamera.spinXZ) * sin(mainCamera.rad * mainCamera.spinY);
	//mainCamera.rotZ = mainCamera.zoom * cos(mainCamera.rad * mainCamera.spinXZ) * sin(mainCamera.rad * mainCamera.spinY);
	

	switch(key)
	{
	// Rotate the 1st person camera using arrow keys
	case GLUT_KEY_LEFT:
		mainCamera.rotX -= 1 * rotateSpeed;
		break;
	case GLUT_KEY_RIGHT:
		mainCamera.rotX += 1 * rotateSpeed;
		break;
	case GLUT_KEY_UP:
		mainCamera.rotY -= 1 * rotateSpeed;
		if (mainCamera.rotY <= -90)
			mainCamera.rotY = -89;
		break;
	case GLUT_KEY_DOWN:
		mainCamera.rotY += 1 * rotateSpeed;
		if (mainCamera.rotY >= 90)
			mainCamera.rotY = 89;
		break;

	// Enable/diable debug mode
	case GLUT_KEY_F1:
		//window.debug_mode = !window.debug_mode;
		window.debug_index = ++window.debug_index % (window.debug_objects.size() + 1);
		break;
	}

	// This allows the camera to rotate about itself... but I need it to rotate around the center
	mainCamera.direction.x = -cos(mainCamera.rotY*(-3.14f/180.0f)) * sin(mainCamera.rotX*(-3.14f/180.0f));
	mainCamera.direction.y = sin(mainCamera.rotY*(-3.14f/180.0f));
	mainCamera.direction.z = -cos(mainCamera.rotY*(-3.14f/180.0f)) * cos(mainCamera.rotX*(-3.14f/180.0f));

	// Right vector
	//mainCamera.right.x = sin(mainCamera.rotX - 3.14f/2.0f);
	//mainCamera.right.y = 0;
	//mainCamera.right.z = cos(mainCamera.rotX - 3.14f/2.0f);

	// Up vector
	//mainCamera.up = cross(mainCamera.right, mainCamera.direction);
}

void DisplayInstructions()
{
	if (window.window_handle == -1)
		return;

	vector<string> * s = &window.instructions;
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glColor3f(1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, window.size.x, 0, window.size.y, 1, 10);
	glViewport(0, 0, window.size.x, window.size.y);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(10, 15 * s->size(), -5.5);
	glScaled(0.1, 0.1, 1.0);
	glLineWidth(1.0);
	for (auto i = s->begin(); i < s->end(); ++i)
	{
		glPushMatrix();
		glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *) (*i).c_str());
		glPopMatrix();
		glTranslated(0, -150, 0);
	}
}

// The scene drawing code is kept separate in case we want to use multiple cameras like in project one
void DrawScene(mat4 & projection_matrix, mat4 & modelview_matrix)
{
	mat4 mv_scaled;


	// Originally, I defined debug mode to draw the axes and grid. Near the end, I switched these over to objects so that they wouldn't be using legacy gl. Then, later, I switched them to have separate key bindings
	// 'a' and 'g' so that we could toggle them individually and use the debug mode to draw our objects individually.
	//if (window.debug_mode)
	//{
	//	// Converted the axes and grid into objects so that we can use modern gl instead of legacy gl
	//	// Draw the main axes
	//	/*glLoadMatrixf(value_ptr(modelview_matrix));
	//	glLineWidth(2.0);
	//	glBegin(GL_LINES);
	//		glColor3f(1,0,0);	glVertex3fv(ORG);	glVertex3fv(XP);
	//		glColor3f(0,1,0);	glVertex3fv(ORG);	glVertex3fv(YP);
	//		glColor3f(0,0,1);	glVertex3fv(ORG);	glVertex3fv(ZP);
	//	glEnd();*/
	//	m = scale(modelview_matrix, vec3(5.0f, 5.0f, 5.0f));
	//	axes->Draw(projection_matrix, m, window.shaders[3], window.size, 0.0f);

	//	// Draw the grid
	//	// It was easier to draw the grid in all positive increments starting from (0,0,0) and then just shift it in order to center it on the modelview_matrix
	//	/*m = translate(modelview_matrix, vec3(-5.0f, 0.0f, -5.0f));
	//	glLoadMatrixf(value_ptr(m));
	//	glLineWidth(1.0);
	//	glColor3f(1,1,1);
	//	glBegin(GL_LINES);
	//	float j;
	//		for (int i=0; i <= GRIDWIDTH*2; i++)
	//		{
	//			j = i/2.0f;
	//			glVertex3f(j,0,0);
	//			glVertex3f(j,0,GRIDWIDTH);
	//			glVertex3f(0,0,j);
	//			glVertex3f(GRIDWIDTH,0,j);
	//		}
	//	glEnd();*/
	//	m = translate(modelview_matrix, vec3(-5.0f, 0.0f, -5.0f));
	//	m = scale(m, vec3(0.5f, 0.5f, 0.5f));
	//	glLineWidth(1.0);
	//	grid->Draw(projection_matrix, m, window.shaders[3], window.size, 0.0f);

	//	// May want to adjust the scale when debugging
	//	m = scale(modelview_matrix, vec3(0.083f, 0.083f, 0.083f));
	//}
	//else
	//{
	//	// Scale from feet to inches
	//	m = scale(modelview_matrix, vec3(0.083f, 0.083f, 0.083f));
	//}

	// Decide whether to draw the axes and/or grid
	if (window.axes)
	{
		mv_scaled = scale(modelview_matrix, vec3(5.0f, 5.0f, 5.0f));
		axes->Draw(projection_matrix, mv_scaled, window.shaders[3], window.size, 0.0f);
	}
	if (window.grid)
	{
		mv_scaled = translate(modelview_matrix, vec3(-5.0f, 0.0f, -5.0f));
		mv_scaled = scale(mv_scaled, vec3(0.5f, 0.5f, 0.5f));
		glLineWidth(1.0);
		grid->Draw(projection_matrix, mv_scaled, window.shaders[3], window.size, 0.0f);
	}

	// If the debug index is one larger than the last item in debug objects, draw the full scene
	if (window.debug_index == window.debug_objects.size())
	{
		mv_scaled = scale(modelview_matrix, vec3(0.083f, 0.083f, 0.083f));
		mat4 m;

		// Draw the stools in arbitrary positions
		//stool1->Draw(window.projection_matrix, window.modelview_matrix, window.size, 0.0f);
		//stool1->Draw(window.projection_matrix, m, window.size, 0.0f);

		walls->Draw(window.projection_matrix, mv_scaled, &brick_shader, window.size, 0.0f);

		m = translate(mv_scaled, vec3(0.0f, 0.0f, -15.0f));
		person->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);
		bar->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);
		m = translate(m, vec3());
		can1->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);
		m = translate(m, vec3(5.0f, 0.0f, 0.0f));
		can2->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);
		m = translate(m, vec3(-10.0f, 0.0f, 0.0f));
		can3->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);

		m = translate(mv_scaled, vec3(0.0f, 0.0f, -10.0f));
		stool1->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);
		m = translate(m, vec3(24.0f, 0.0f, 0.0f));
		stool2->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);
		m = translate(m, vec3(-48.0f, 0.0f, 0.0f));
		stool3->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);

		/*glutSwapBuffers();
		glutPostRedisplay();*/
	}
	// Otherwise, draw just the current object in the debug objects list
	else
	{
		mv_scaled = scale(modelview_matrix, vec3(0.15f, 0.15f, 0.15f));
		window.debug_objects[window.debug_index]->Draw(window.projection_matrix, mv_scaled, window.shaders[window.shader_index], window.size, 0.0f);
	}
}

void DisplayFunc()
{
	// Started modifying the code for the camera movement but never got a chance to finish. So decided to just comment it out and go back to what I had before
	//if (window.window_handle == -1)
	//	return;
	//
	//if (mainCamera.spinY < 1.0f) mainCamera.spinY = 1.0f;
	//if (mainCamera.spinY > 179.0f) mainCamera.spinY = 179.0f;
	//
	//if (mainCamera.lens < 30) mainCamera.lens = 30.0f;
	//if (mainCamera.lens > 120) mainCamera.lens = 120.0f;

	//assert(stool1 != NULL);
	//glEnable(GL_CULL_FACE);
	//// Dob't need to change the front face anymore since I fixed the vertex winding to be CCW
	////glFrontFace(GL_CW);
	//glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);
	//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glViewport(0, 0, window.size.x, window.size.y);
	//glMatrixMode(GL_PROJECTION);
	//window.projection_matrix = perspective(mainCamera.lens,  window.window_aspect, 1.0f, 1000.0f);
	//glLoadMatrixf(value_ptr(window.projection_matrix));
	//glMatrixMode(GL_MODELVIEW);
	//// This allows the user to rotate the camera around the target \ made this a bit easier to move around with
	//window.modelview_matrix = rotate(mat4(1.0f), 0.0f, vec3(0.0f, 1.0f, 0.0f));
	//window.modelview_matrix = rotate(window.modelview_matrix, 0.0f, vec3(1.0f, 0.0f, 0.0f));
	//// This allows the user to strafe the camera \ same as above
	//vec4 eye = window.modelview_matrix * vec4(mainCamera.rotX, mainCamera.rotY, mainCamera.rotZ, 1.0f);
	//vec4 target = window.modelview_matrix * vec4(mainCamera.tranX, mainCamera.tranY, mainCamera.tranZ, 1.0f);
	//window.modelview_matrix = lookAt(vec3(eye), vec3(target), vec3(0.0f, 1.0f, 0.0f));

	//DrawScene(window.projection_matrix, window.modelview_matrix);
	//DisplayInstructions();

	//glutSwapBuffers();
	//glutPostRedisplay();


	if (window.window_handle == -1)
		return;

	assert(stool1 != NULL);
	glEnable(GL_CULL_FACE);
	// Dob't need to change the front face anymore since I fixed the vertex winding to be CCW
	//glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, window.size.x, window.size.y);
	//glMatrixMode(GL_PROJECTION);
	window.projection_matrix = perspective(mainCamera.zoom,  window.window_aspect, 1.0f, 25.0f);
	//glLoadMatrixf(value_ptr(window.projection_matrix));
	//glMatrixMode(GL_MODELVIEW);

	// If camera is in freemode, rotate the camera like in an FPS
	if (mainCamera.freemode)
	{
		window.modelview_matrix = lookAt(mainCamera.position, mainCamera.position + mainCamera.direction, mainCamera.up);
	}
	// Otherwise, constrain the camera to rotate around a target point
	else
	{
		// This allows the user to rotate the camera around the target
		window.modelview_matrix = rotate(mat4(1.0f), mainCamera.rotX, vec3(0.0f, 1.0f, 0.0f));
		window.modelview_matrix = rotate(window.modelview_matrix, mainCamera.rotY, vec3(1.0f, 0.0f, 0.0f));
		// This allows the user to strafe the camera
		vec4 eye = window.modelview_matrix * vec4(mainCamera.tranX, mainCamera.tranY, mainCamera.tranZ + 15.0f, 1.0f);
		vec4 target = window.modelview_matrix * vec4(mainCamera.tranX, mainCamera.tranY, mainCamera.tranZ, 1.0f);
		window.modelview_matrix = lookAt(vec3(eye), vec3(target), vec3(0.0f, 1.0f, 0.0f));
	}

	DrawScene(window.projection_matrix, window.modelview_matrix);
	DisplayInstructions();

	glutSwapBuffers();
	glutPostRedisplay();
}

int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(1024, 1024);
	glutInitWindowPosition(20, 20);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	window.window_handle = glutCreateWindow("1st Person View");
	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);
	glutCloseFunc(CloseFunc);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	window.wireframe = false;
	window.normals = false;
	window.points = false;
	window.debug_mode = false;

	window.instructions.push_back("Project 2 - UW-Madison - CS 559");
	window.instructions.push_back("Eric Satterness and Garett Handel");
	window.instructions.push_back("");
	window.instructions.push_back("UP/DOWN/LEFT/RIGHT    Rotate the camera");
	window.instructions.push_back("I/K                   Move camera forward/backward");
	window.instructions.push_back("J/L                   Move camera left/right");
	window.instructions.push_back("U/O                   Move camera up/down");
	window.instructions.push_back("+/-                   Zoom camera in/out");
	window.instructions.push_back("C                     Toggle the camera mode");
	window.instructions.push_back("N                     Toggle normals");
	window.instructions.push_back("P                     Toggle points");
	window.instructions.push_back("W                     Toggle wireframe");
	window.instructions.push_back("A                     Toggle axes");
	window.instructions.push_back("G                     Toggle grid");
	window.instructions.push_back("F1                    Switch debug/relase mode");
	window.instructions.push_back("X                     Exit");

	// Initialize 1st person camera
	/*mainCamera.rotX = 0.0;
	mainCamera.rotY = 0.0;
	mainCamera.tranX = 0.0;
	mainCamera.tranZ = 0.0;
	mainCamera.zoom = 40.0;
	mainCamera.position = vec3(0.0f, 0.0f, 0.0f);
	mainCamera.direction = vec3(0.0f, 0.0f, 1.0f);
	mainCamera.up = vec3(0.0f, 1.0f, 0.0f);
	mainCamera.right = vec3(0.0f, 0.0f, 0.0f);*/
	ResetCamera();

	if (glewInit() != GLEW_OK)
	{
		cerr << "GLEW failed to initialize." << endl;
		return 0;
	}

	if (!phong_shader.Initialize("phong_shader.vert", "phong_shader.frag"))
		return 0;
	if (!gouraud_shader.Initialize("gouraud_shader.vert", "gouraud_shader.frag"))
		return 0;
	if (!flat_shader.Initialize("flat_shader.vert", "flat_shader.frag"))
		return 0;
	if (!colored_shader.Initialize("colored_shader.vert", "colored_shader.frag"))
		return 0;
	if (!basic_shader.Initialize("basic_shader.vert", "basic_shader.frag"))
		return 0;
	if (!brick_shader.Initialize("brick_shader.vert", "brick_shader.frag"))
		return 0;
	if (!phong_attenuated_shader.Initialize("phong_shader.vert", "phong_attenuated_shader.frag"))
		return 0;
	window.shaders.push_back(&phong_shader);
	window.shaders.push_back(&gouraud_shader);
	window.shaders.push_back(&flat_shader);
	window.shaders.push_back(&colored_shader);
	window.shaders.push_back(&basic_shader);
	window.shaders.push_back(&brick_shader);
	window.shaders.push_back(&phong_attenuated_shader);

	// Now that we are sending a shader pointer to the draw methods, we don't need to pass in the shader files themselves
	/*phong_shader.vert = "phong_shader.vert";
	phong_shader.frag = "phong_shader.frag";
	gouraud_shader.vert = "basic_shader.vert";
	gouraud_shader.frag = "basic_shader.frag";

	window.shader_index = 0;
	window.shaders.push_back(&phong_shader);
	window.shaders.push_back(&gouraud_shader);*/

	/*stool1 =  new Stool();
	if (!stool1->Initialize())
	{
		stool1->TakeDown();
		return 0;
	}
	stool2 =  new Stool();
	if (!stool2->Initialize())
	{
		stool2->TakeDown();
		return 0;
	}
	if (!stool1->SetShader(window.shaders[0]->vert, window.shaders[0]->frag))
		return 0;
	if (!stool2->SetShader(window.shaders[0]->vert, window.shaders[0]->frag))
		return 0;*/

	// Any objects that we don't want to have normals displayed should be kept out of the main objects list.
	// This also means that they will not be affected by the shader stepping.
	axes = new Axes();
	if (!axes->Initialize())
	{
		axes->TakeDown();
		delete axes;
		return 0;
	}
	grid = new Grid();
	if (!grid->Initialize())
	{
		grid->TakeDown();
		delete grid;
		return 0;
	}
	walls =  new Walls();
	if (!walls->Initialize())
	{
		walls->TakeDown();
		delete walls;
		return 0;
	}

	stool1 = new Stool();
	stool2 = new Stool();
	stool3 = new Stool();
	person =  new Person();
	bar =  new Bar();
	can1 =  new Can();
	can2 =  new Can();
	can3 =  new Can();

	window.objects.push_back(stool1);
	window.objects.push_back(stool2);
	window.objects.push_back(stool3);
	window.objects.push_back(person);
	window.objects.push_back(bar);
	window.objects.push_back(can1);
	window.objects.push_back(can2);
	window.objects.push_back(can3);

	window.debug_objects.push_back(stool1);
	window.debug_objects.push_back(person);
	window.debug_objects.push_back(bar);
	window.debug_objects.push_back(can1);
	//window.debug_objects.push_back(walls);

	for (int i = 0; i < (int)window.objects.size(); i++)
	{
		if (!window.objects[i]->Initialize())
		{
			window.objects[i]->TakeDown();
			delete window.objects[i];
			return 0;
		}
	}

	glutMainLoop();
}