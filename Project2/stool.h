#pragma once
#include "object.h"
#include "shader.h"
#include "shapes.h"
#include "vectorExtensions.h"

#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>

class Stool : public Object
{
private:
	typedef Object super;
	static const vec3 CENTER, UP, RIGHT;
	static const float LEG_OFFSET, LEG_WIDTH, LEG_HEIGHT;
	static const float SEAT_RADIUS_BOT, SEAT_RADIUS_TOP, SEAT_THICKNESS, SEAT_OFFSET;
	static const float ROD_RADIUS, ROD_HEIGHT;
	static const float RING_RADIUS_INNER, RING_RADIUS_OUTER, RING_OFFSET;

	//int shader_index;

	//BAD DESIGN WARNING...
	//To improve O.O. design, the triangle should NOT be aware of what shader or texture is
	//being used  to render  it.  Therefore, none of the lines below should really be here.

	//Shader phong_shader;
	//Shader gouraud_shader;
	//Shader flat_shader;
	//Shader colored_shader;
	//Shader basic_shader;
	////TexturedShader basic_texture_shader;
	//std::vector<Shader *> shaders;
	//Shader * shader;

	void BuildNormalVisualizationGeometry();
	void InitLeg(vec3 center, vec3 up, vec3 right);
	void InitDiskSupport(vec3 center, vec3 up, vec3 right, float radiusTop, float radiusBot, float height);
	void InitRingSupport();
	void InitSeat();
	void InitSeatRod();

public:
	Stool();
	//virtual bool Initialize(char * vertex_shader_file, char * fragment_shader_file);
	virtual bool Initialize();
	virtual void Draw(const glm::mat4& projection, glm::mat4 modelview, Shader * shader, const glm::ivec2 & size, const float time = 0);
	void TakeDown();
	//void StepShader();
	//bool SetShader(char * vertex_shader_file, char * fragment_shader_file);
};