#pragma once
#include <glad/glad.h>
#include "dependencies/glm/glm.hpp"

class Sprite
{
public:
	Sprite(const char* path, GLuint shaderID);
	void Draw();
	void setScale(glm::vec3 scale);
	void setTranslate(glm::vec3 translate);
	void deleteVertexArray();
	void setSpriteSheet(int cols, int rows);
	void setScrollOffset(glm::vec2 offset);
	virtual void update(float deltaTime);
	void setVelocity(const glm::vec3& velocity);
	glm::vec3 getVelocity() const;
protected:
	glm::vec3 m_translate;
	int m_sheetCols = 1;
	int m_sheetRows = 1;
	int m_frameIndex = 0;
	int m_animationRow = 0;
	int m_currentFrame = 0;
	glm::vec2 m_scrollOffset = glm::vec2(0.0f);
	float m_animTimer = 0.0f;
	float m_frameDuration = 0.1f;
	glm::vec3 m_velocity = glm::vec3(0.0f);
	bool updateAnimationFromVelocity();
private:
	GLuint m_TextureID;
	GLuint VAO;
	glm::vec3 m_scale;
	GLuint shaderID;
};

