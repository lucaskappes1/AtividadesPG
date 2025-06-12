#pragma once
#include "Sprite.h"
class ControllableCharacter : public Sprite
{
public:
	ControllableCharacter(const char* path, GLuint shaderId) : Sprite(path, shaderId) 
	{
		m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	void update(float deltaTime);

private:
	
};

