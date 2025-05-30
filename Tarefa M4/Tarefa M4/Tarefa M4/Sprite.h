#pragma once
#include <glad/glad.h>
#include "dependencies/glm/glm.hpp"

class Sprite
{
public:
	Sprite(const char* path, GLuint shaderId);
	void Draw();
	void setScale(glm::vec3 scale);
	void setTranslate(glm::vec3 translate);
	void deleteVertexArray();
private:
	GLuint m_TextureID;
	GLuint VAO;
	glm::vec3 m_translate;
	glm::vec3 m_scale;
	GLuint shaderId;
};

