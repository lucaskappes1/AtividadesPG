#pragma once
#include <iostream>
#include <string>
#include <assert.h>
#include "dependencies/glad/glad.h"
#include "dependencies/glfw/include/GLFW/glfw3.h"
#include "dependencies/glm/glm.hpp"
#include "dependencies/glm/gtc/matrix_transform.hpp"
#include "dependencies/glm/gtc/type_ptr.hpp"
#include <vector>
#include <random>
#include <cmath>
#include "ControllableCharacter.h"

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Protótipos das funções
int setupShader();

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = R"(#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 colors;
layout (location = 2) in vec2 texture_mapping;

out vec2 texture_coordinates;
out vec3 color_values;

uniform mat4 projection;
uniform mat4 model;
uniform vec2 scrollOffset;

uniform ivec2 sheetSize;   
uniform int frameIndex;

void main()
{
    int column = frameIndex % sheetSize.x;
    int row    = frameIndex / sheetSize.x;
    vec2 cellSize = vec2(1.0) / vec2(sheetSize);
    vec2 frameOffset = vec2(column, row) * cellSize;
    texture_coordinates = texture_mapping * cellSize + frameOffset + scrollOffset;
    color_values = colors;
    gl_Position = projection * model * vec4(position, 1.0);
})";

//Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = R"(#version 400
in vec2 texture_coordinates;
in vec3 color_values;
out vec4 color;

uniform sampler2D spriteTexture;

void main()
{
    vec4 texColor = texture(spriteTexture, texture_coordinates);
    color = texColor;
}
)";

std::vector<Sprite*> sprites;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Triangulo! -- Lucas Kappes", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported " << version << std::endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	glUseProgram(shaderID);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enviando a cor desejada (vec4) para o fragment shader
	// Utilizamos a variáveis do tipo uniform em GLSL para armazenar esse tipo de info
	// que não está nos buffers
	GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

	//Matriz de projeção paralela ortográfica
	glm::mat4 projection = glm::ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

	sprites.push_back(new Sprite("assets/orig.png", shaderID));
	sprites[0]->setScale(glm::vec3(800, 600, 0));
	sprites[0]->setTranslate(glm::vec3(400, 300, 0));

	sprites.push_back(new Sprite("assets/samurai.png", shaderID));
	sprites[1]->setScale(glm::vec3(100, 100, 0));
	sprites[1]->setTranslate(glm::vec3(100, 100, 0));

	sprites.push_back(new Sprite("assets/hood_archer.png", shaderID));
	sprites[2]->setScale(glm::vec3(100, 100, 0));
	sprites[2]->setTranslate(glm::vec3(300, 100, 0));

	sprites.push_back(new Sprite("assets/monster0.png", shaderID));
	sprites[3]->setScale(glm::vec3(150, 150, 0));
	sprites[3]->setTranslate(glm::vec3(200, 400, 0));

	sprites.push_back(new Sprite("assets/monster1.png", shaderID));
	sprites[4]->setScale(glm::vec3(180, 180, 0));
	sprites[4]->setTranslate(glm::vec3(600, 300, 0));

	sprites.push_back(new Sprite("assets/monster2.png", shaderID));
	sprites[5]->setScale(glm::vec3(200, 200, 0));
	sprites[5]->setTranslate(glm::vec3(600, 100, 0));

	sprites.push_back(new ControllableCharacter("assets/Sword_Run_full.png", shaderID));
	sprites[6]->setScale(glm::vec3(100, 100, 0));
	sprites[6]->setTranslate(glm::vec3(400, 400, 0));
	sprites[6]->setSpriteSheet(8, 4); 
	sprites[6]->setVelocity(glm::vec3(0, 0, 0));

	float lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;
   
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		for (int i = 0; i < sprites.size(); i++) {
			sprites[i]->update(deltaTime);
			sprites[i]->Draw();
		}

		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	for (int i = 0; i < sprites.size(); i++)
	{
		sprites[i]->deleteVertexArray();
	}
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		sprites[6]->setVelocity(sprites[6]->getVelocity() + glm::vec3(0, 1, 0));
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		sprites[6]->setVelocity(sprites[6]->getVelocity() + glm::vec3(0, -1, 0));
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		sprites[6]->setVelocity(sprites[6]->getVelocity() + glm::vec3(1, 0, 0));
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		sprites[6]->setVelocity(sprites[6]->getVelocity() + glm::vec3(-1, 0, 0));

	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		sprites[6]->setVelocity(sprites[6]->getVelocity() + glm::vec3(0, -1, 0));
	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		sprites[6]->setVelocity(sprites[6]->getVelocity() + glm::vec3(0, 1, 0));
	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		sprites[6]->setVelocity(sprites[6]->getVelocity() + glm::vec3(-1, 0, 0));
	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		sprites[6]->setVelocity(sprites[6]->getVelocity() + glm::vec3(1, 0, 0));
}

int setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

}