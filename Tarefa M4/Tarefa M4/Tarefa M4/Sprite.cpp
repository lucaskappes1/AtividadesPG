#include "Sprite.h"
#include "stb/stb_image.h"
#include "dependencies/glm/gtc/matrix_transform.hpp"
#include "dependencies/glm/gtc/type_ptr.hpp"
#include <iostream>

Sprite::Sprite(const char* path, GLuint shaderId)
{
	//Gerar a textura
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, numChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &numChannels, STBI_rgb_alpha);
	if (data)
	{
		GLenum format = numChannels == 4 ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Erro ao carregar textura: " << path << std::endl;
	}
	stbi_image_free(data);

	//Gerar a geometria
	GLuint VBO;
	float vertices[] = {
		//posições			//cores				//coordenadas de textura
		0.5, 0.5, 0.0,		0.0, 0.0, 0.0,		1.0, 1.0,
		-0.5, -0.5, 0.0,	0.0, 0.0, 0.0,		0.0, 0.0,
		0.5, -0.5, 0.0,		0.0, 0.0, 0.0,		1.0, 0.0,
		-0.5, 0.5, 0.0,		0.0, 0.0, 0.0,		0.0, 1.0
	};
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//atributo de posição
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//atributo de cor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//atributo de coordenada de textura
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	unsigned int indices[] = {
		0, 1, 2,//primeiro triangulo
		0, 3, 1 //segundo triangulo
	};
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	this->shaderId = shaderId;

}

void Sprite::Draw()
{
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glBindVertexArray(VAO);
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, m_translate);
	model = glm::scale(model, m_scale);
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "model"), 1, GL_FALSE, value_ptr(model));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Sprite::setScale(glm::vec3 scale)
{
	m_scale = scale;
}

void Sprite::setTranslate(glm::vec3 translate)
{
	m_translate = translate;
}

void Sprite::deleteVertexArray()
{
	glDeleteVertexArrays(1, &VAO);
}
