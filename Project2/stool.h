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
	static const float LEG_OFFSET;
	static const float LEG_WIDTH;
	static const float LEG_HEIGHT;

	int shader_index;

	//      BAD DESIGN WARNING...
	//      To improve O.O. design, the triangle should NOT be aware of what shader or texture is
	//      being used  to render  it.  Therefore, none of the lines below should really be here.

	Shader pattern_shader;
	Shader basic_shader;
	Shader colored_shader;
	Shader play_shader;
	//TexturedShader basic_texture_shader;
	std::vector<Shader *> shaders;


	void InitLeg(vec3 center, vec3 up, vec3 right);
	void InitDiskSupport(vec3 center, vec3 up, vec3 right, float radiusTop, float radiusBot, float height);
	//void InitRingSupport(vec3 center, vec3 up, vec3 right, float innerRadius, float outerRadius);
	void InitRingSupport();

public:
	Stool();
	virtual bool Initialize();
    virtual void Draw(const glm::mat4& projection, glm::mat4 modelview, const glm::ivec2 & size, const float time = 0);
    void TakeDown();
    void StepShader();

	bool drawNormals, drawPoints;
};