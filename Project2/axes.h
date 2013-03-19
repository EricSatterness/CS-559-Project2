#pragma once
#include "object.h"

using namespace std;
using namespace glm;

class Axes : public Object
{
private:
	typedef Object super;
	static const vec3 ORIGIN, X_AXIS, Y_AXIS, Z_AXIS;

protected:
	std::vector<VertexAttributesPC> vertices;

public:
	Axes();
	virtual bool Initialize();
	virtual void Draw(const glm::mat4& projection, glm::mat4 modelview, Shader * shader, const glm::ivec2 & size, const float time = 0);
	void TakeDown();
};