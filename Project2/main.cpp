#include <iostream>
#include <assert.h>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include "vertexattributes.h"
#include "object.h"
#include "stool.h"
//#include "shader.h"

// Useful links
//http://www.arcsynthesis.org/gltut/index.html

using namespace std;
using namespace glm;

#pragma region Window/Camera Structs
// Keeps track of data relevant to each individual window
struct WindowData
{
	int window_handle;
	ivec2 size;
	float window_aspect;
	mat4 projection_matrix, modelview_matrix;
} window;

// Keeps track of the 1st person camera data
struct CameraData
{
	float rotX, rotY;
	float tranX, tranZ;
	float zoom;
} mainCamera;
#pragma endregion

#pragma region Constants
// These represent the axes
const float	ORG[3] = {0,0,0},
			XP[3] = {5,0,0},
			YP[3] = {0,5,0},
			ZP[3] = {0,0,5};
// Used for constructing the floor grid. Units are in feet
const float GRIDWIDTH = 10.0;
#pragma endregion

#pragma region Variables
// Move speed is in feet
float moveSpeed = 0.25f;
// Rotate speed is in degrees
float rotateSpeed = 1.0f;
// Zoom speed modifies the camera fov in units of feet
float zoomSpeed = 0.5f;

// Stool objects to be drawn in the scene
Stool *stool1;
//*stool2, *stool3;
#pragma endregion

void CloseFunc()
{
	window.window_handle = -1;

	if (stool1 != NULL)
	{
		stool1->TakeDown();
		delete stool1;
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
	// IJKL strafe the camera
	case 'i':
		mainCamera.tranZ -= 1 * moveSpeed;
		break;
	case 'k':
		mainCamera.tranZ += 1 * moveSpeed;
		break;
	case 'j':
		mainCamera.tranX -= 1 * moveSpeed;
		break;
	case 'l':
		mainCamera.tranX += 1 * moveSpeed;
		break;

	// Zoom camera in and out
	case 'n':
		mainCamera.zoom -= 1 * zoomSpeed;
		break;
	case 'm':
		mainCamera.zoom += 1 * zoomSpeed;
		break;

	case 's':
		stool1->StepShader();
		break;

	case 'x':
	case 27:
		glutLeaveMainLoop();
		return;
	}

	// Ensure camera can't strafe indefinitely
	if (mainCamera.tranZ > 15) mainCamera.tranZ = 15;
	if (mainCamera.tranZ < -15) mainCamera.tranZ = -15;
	if (mainCamera.tranX > 15) mainCamera.tranX = 15;
	if (mainCamera.tranX < -15) mainCamera.tranX = -15;

	// Ensure camera can't zoom indefinitely
	if (mainCamera.zoom > 60) mainCamera.zoom = 60;
	if (mainCamera.zoom < 10) mainCamera.zoom = 10;
}

void SpecialFunc(int key, int x, int y)
{
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
	}
}

void DrawScene(mat4 & projection_matrix, mat4 & modelview_matrix)
{
	/*// Draw the main axes
	glLoadMatrixf(value_ptr(modelview_matrix));
	glLineWidth(2.0);
	glBegin(GL_LINES);
		glColor3f(1,0,0);	glVertex3fv(ORG);	glVertex3fv(XP);
		glColor3f(0,1,0);	glVertex3fv(ORG);	glVertex3fv(YP);
		glColor3f(0,0,1);	glVertex3fv(ORG);	glVertex3fv(ZP);
	glEnd();

	// Draw the grid
	// It was easier to draw the grid in all positive increments starting from (0,0,0) and then just shift it in order to center it on the modelview_matrix
	mat4 m = translate(modelview_matrix, vec3(-5.0f, 0.0f, -5.0f));
	glLoadMatrixf(value_ptr(m));
	glLineWidth(1.0);
	glColor3f(1,1,1);
	glBegin(GL_LINES);
	float j;
		for (int i=0; i <= GRIDWIDTH*2; i++)
		{
			j = i/2.0f;
			glVertex3f(j,0,0);
			glVertex3f(j,0,GRIDWIDTH);
			glVertex3f(0,0,j);
			glVertex3f(GRIDWIDTH,0,j);
		}
	glEnd();*/

	// Scale from feet to inches
	mat4 m = scale(modelview_matrix, vec3(0.083f, 0.083f, 0.083f));

	// Make scale a little bigger while debugging
	//mat4 m = scale(modelview_matrix, vec3(0.15f, 0.15f, 0.15f));

	// Draw the stools in arbitrary positions
	//stool1->Draw(window.projection_matrix, window.modelview_matrix, window.size, 0.0f);
	stool1->Draw(window.projection_matrix, m, window.size, 0.0f);

	glutSwapBuffers();
	glutPostRedisplay();
}

void DisplayFunc()
{
	if (window.window_handle == -1)
		return;

	assert(stool1 != NULL);
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, window.size.x, window.size.y);
	glMatrixMode(GL_PROJECTION);
	window.projection_matrix = perspective(mainCamera.zoom,  window.window_aspect, 1.0f, 10.0f);
	glLoadMatrixf(value_ptr(window.projection_matrix));
	glMatrixMode(GL_MODELVIEW);
	// This allows the user to rotate the camera around the target
	window.modelview_matrix = rotate(mat4(1.0f), mainCamera.rotX, vec3(0.0f, 1.0f, 0.0f));
	window.modelview_matrix = rotate(window.modelview_matrix, mainCamera.rotY, vec3(1.0f, 0.0f, 0.0f));
	// This allows the user to strafe the camera
	vec4 eye = window.modelview_matrix * vec4(mainCamera.tranX, 0.0f, mainCamera.tranZ + 8.0f, 1.0f);
	vec4 target = window.modelview_matrix * vec4(mainCamera.tranX, 0.0f, mainCamera.tranZ, 1.0f);
	window.modelview_matrix = lookAt(vec3(eye), vec3(target), vec3(0.0f, 1.0f, 0.0f));

	DrawScene(window.projection_matrix, window.modelview_matrix);
}

int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(1024, 1024);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	window.window_handle = glutCreateWindow("1st Person View");
	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);
	glutCloseFunc(CloseFunc);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	// Initialize 1st person camera
	mainCamera.rotX = 0.0;
	mainCamera.rotY = 0.0;
	mainCamera.tranX = 0.0;
	mainCamera.tranZ = 0.0;
	mainCamera.zoom = 40.0;

	if (glewInit() != GLEW_OK)
	{
		cerr << "GLEW failed to initialize." << endl;
		return 0;
	}

	stool1 =  new Stool();
	if (!stool1->Initialize())
	{
		stool1->TakeDown();
		return 0;
	}

	glutMainLoop();
}