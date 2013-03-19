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

class Environment : public Object
{
private:
	typedef Object super;
	static const vec3 CENTER, UP, RIGHT;
	static const float LEG_OFFSET, LEG_WIDTH, LEG_HEIGHT;
    static const float CAN_HEIGHT, CAN_RADIUS;
    static const float BAR_HEIGHT, BAR_WIDTH, BAR_DEPTH;
    static const float WALLS_WIDTH, WALLS_HEIGHT;
    static const float HEAD_RADIUS, HEAD_LENGTH, SPINE_HEIGHT;

	void BuildNormalVisualizationGeometry();
	void InitLeg(vec3 center, vec3 up, vec3 right);
    void InitWalls();

public:
	Environment();
	virtual bool Initialize();
	virtual void Draw(const glm::mat4& projection, glm::mat4 modelview, Shader * shader, const glm::ivec2 & size, const float time = 0);
	void TakeDown();
};