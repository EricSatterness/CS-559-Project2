#include <iostream>
#include <assert.h>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stool.h"
#include "environment.h"
#include "person.h"
#include "bar.h"
#include "can.h"
#include "walls.h"

// Useful links
//http://www.arcsynthesis.org/gltut/index.html

using namespace std;
using namespace glm;

#define DEBUG 1

#pragma region Data Structs
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
	bool wireframe, normals, points, debug_mode;
	int shader_index;
	//std::vector<ShaderFiles *> shaders;
	std::vector<Shader *> shaders;
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
Stool *stool2;
Stool *stool3;
Person *person;
Bar *bar;
Can *can1;
Can *can2;
Can *can3;
Walls *walls;
//*stool2, *stool3;
#pragma endregion

void CloseFunc()
{
	window.window_handle = -1;

	if (walls != NULL)
	{
		walls->TakeDown();
		delete walls;
	}
	if (person != NULL)
	{
		person->TakeDown();
		delete person;
	}
	if (bar != NULL)
	{
		bar->TakeDown();
		delete bar;
	}
	if (can1 != NULL)
	{
		can1->TakeDown();
		delete can1;
	}
	if (can2 != NULL)
	{
		can2->TakeDown();
		delete can2;
	}
	if (can3 != NULL)
	{
		can3->TakeDown();
		delete can3;
	}
	if (stool1 != NULL)
	{
		stool1->TakeDown();
		delete stool1;
	}
	if (stool2 != NULL)
	{
		stool2->TakeDown();
		delete stool2;
	}
	if (stool3 != NULL)
	{
		stool3->TakeDown();
		delete stool3;
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
		mainCamera.rotX -= 1 * moveSpeed;
		break;
	case 'l':
		mainCamera.tranX += 1 * moveSpeed;
		mainCamera.rotX += 1 * moveSpeed;
		break;

	// Zoom camera in and out
	case '=':
		mainCamera.zoom -= 1 * zoomSpeed;
		break;
	case '-':
		mainCamera.zoom += 1 * zoomSpeed;
		break;

	// Step through shaders
	case 's':
		window.shader_index = ++window.shader_index % window.shaders.size();
		//stool1->SetShader(window.shaders[window.shader_index]->frag, window.shaders[window.shader_index]->vert);
		//stool1->StepShader();
		break;

	// Turn on/off wireframe, normals, and points
	case 'w':
		window.wireframe = !window.wireframe;
		break;
	case 'n':
		stool1->EnableNormals(window.normals = !window.normals);
		break;
	case 'p':
		stool1->EnablePoints(window.points = !window.points);
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

	// Enable/diable debug mode
	case GLUT_KEY_F1:
		window.debug_mode = !window.debug_mode;
		break;
	}
}

void DrawScene(mat4 & projection_matrix, mat4 & modelview_matrix)
{
	mat4 m;

	if (window.debug_mode)
	{
		// Draw the main axes
		glLoadMatrixf(value_ptr(modelview_matrix));
		glLineWidth(2.0);
		glBegin(GL_LINES);
			glColor3f(1,0,0);	glVertex3fv(ORG);	glVertex3fv(XP);
			glColor3f(0,1,0);	glVertex3fv(ORG);	glVertex3fv(YP);
			glColor3f(0,0,1);	glVertex3fv(ORG);	glVertex3fv(ZP);
		glEnd();

		// Draw the grid
		// It was easier to draw the grid in all positive increments starting from (0,0,0) and then just shift it in order to center it on the modelview_matrix
		m = translate(modelview_matrix, vec3(-5.0f, 0.0f, -5.0f));
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
		glEnd();

		// Make scale a little bigger while debugging
		m = scale(modelview_matrix, vec3(0.083f, 0.083f, 0.083f));
	}
	else
	{
		// Scale from feet to inches
		m = scale(modelview_matrix, vec3(0.083f, 0.083f, 0.083f));
	}

	// Draw the stools in arbitrary positions
	//stool1->Draw(window.projection_matrix, window.modelview_matrix, window.size, 0.0f);
	//stool1->Draw(window.projection_matrix, m, window.size, 0.0f);
	
	walls->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);
	person->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);
	bar->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);
	can1->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);
	m = translate(m, vec3(5.0f, 0.0f, 0.0f));
	can2->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);
	m = translate(m, vec3(-10.0f, 0.0f, 0.0f));
	can3->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);
	m = translate(m, vec3(5.0f, 0.0f, 0.0f));
	// When another stool is drawn, so is the environment surrounding it. Need to build it once, not for each stool.
	// ^Fixed this by taking it out of the stool class...
	m = translate(m, vec3(0.0f, 0.0f, -24.0f));
	stool1->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);
	m = translate(m, vec3(24.0f, 0.0f, 0.0f));
	stool2->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);
	m = translate(m, vec3(-48.0f, 0.0f, 0.0f));
	stool3->Draw(window.projection_matrix, m, window.shaders[window.shader_index], window.size, 0.0f);

	glutSwapBuffers();
	glutPostRedisplay();
}

void DisplayFunc()
{
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
	glMatrixMode(GL_PROJECTION);
	window.projection_matrix = perspective(mainCamera.zoom,  window.window_aspect, 1.0f, 1000.0f);
	glLoadMatrixf(value_ptr(window.projection_matrix));
	glMatrixMode(GL_MODELVIEW);
	// This allows the user to rotate the camera around the target
	window.modelview_matrix = rotate(mat4(1.0f), 0.0f, vec3(0.0f, 1.0f, 0.0f));
	window.modelview_matrix = rotate(window.modelview_matrix, 0.0f, vec3(1.0f, 0.0f, 0.0f));
	// This allows the user to strafe the camera
	vec4 eye = window.modelview_matrix * vec4(mainCamera.rotX, mainCamera.rotY, mainCamera.tranZ + 8.0f, 1.0f);
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

	window.wireframe = false;
	window.normals = false;
	window.points = false;
	window.debug_mode = false;

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

	Shader phong_shader, gouraud_shader, flat_shader, colored_shader, basic_shader;
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
	window.shaders.push_back(&phong_shader);
	window.shaders.push_back(&gouraud_shader);
	window.shaders.push_back(&flat_shader);
	window.shaders.push_back(&colored_shader);
	window.shaders.push_back(&basic_shader);

	/*phong_shader.vert = "phong_shader.vert";
	phong_shader.frag = "phong_shader.frag";
	gouraud_shader.vert = "basic_shader.vert";
	gouraud_shader.frag = "basic_shader.frag";

	window.shader_index = 0;
	window.shaders.push_back(&phong_shader);
	window.shaders.push_back(&gouraud_shader);*/

	stool1 =  new Stool();
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
	stool3 =  new Stool();
	if (!stool3->Initialize())
	{
		stool3->TakeDown();
		return 0;
	}

	walls =  new Walls();
	if (!walls->Initialize())
	{
		walls->TakeDown();
		return 0;
	}

	person =  new Person();
	if (!person->Initialize())
	{
		person->TakeDown();
		return 0;
	}
	
	bar =  new Bar();
	if (!bar->Initialize())
	{
		bar->TakeDown();
		return 0;
	}
	
	can1 =  new Can();
	if (!can1->Initialize())
	{
		can1->TakeDown();
		return 0;
	}
	can2 =  new Can();
	if (!can2->Initialize())
	{
		can2->TakeDown();
		return 0;
	}
	can3 =  new Can();
	if (!can3->Initialize())
	{
		can3->TakeDown();
		return 0;
	}

	//if (!stool1->SetShader(window.shaders[0]->vert, window.shaders[0]->frag))
	//	return 0;
	//if (!stool2->SetShader(window.shaders[0]->vert, window.shaders[0]->frag))
	//	return 0;

	glutMainLoop();
}