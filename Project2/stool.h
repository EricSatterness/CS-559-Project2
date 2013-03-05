#pragma once
#include "object.h"
#include "shader.h"
#include "shapes.h"

#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Stool : public Object
{
private:
	typedef Object super;
	static const float SCALE_FACTOR;

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

public:
	Stool();
	virtual bool Initialize();
    virtual void Draw(const glm::mat4& projection, glm::mat4 modelview, const glm::ivec2 & size, const float time = 0);
    void TakeDown();
    void StepShader();
	void InitLeg();
};