#pragma once
#include "object.h"

using namespace std;
using namespace glm;

class Grid : public Object
{
private:
	typedef Object super;
	static const float GRID_WIDTH;
	static const vec3 GRID_COLOR;

protected:
	std::vector<VertexAttributesPC> vertices;

public:
	Grid();
	virtual bool Initialize();
	virtual void Draw(const glm::mat4& projection, glm::mat4 modelview, Shader * shader, const glm::ivec2 & size, const float time = 0);
	void TakeDown();
};