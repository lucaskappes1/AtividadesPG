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
// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Protótipos das funções
int setupShader();
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2);
void generateQuads();

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = "#version 400\n"
"layout (location = 0) in vec3 position;\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
"gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";

//Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = "#version 400\n"
"uniform vec4 inputColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = inputColor;\n"
"}\n\0";

struct Quad {
	glm::vec3 bottom_left_position;
	GLuint height = 30;		//com esses valores teremos 20 retangulos na vertical e 20 na horizontal
	GLuint width = 40;
	glm::vec3 dimensions = glm::vec3(40.0, 30.0, 0.0);
	glm::vec4 color;
};

std::vector<Quad> quads;

std::random_device rd;
std::mt19937 gen(rd());

int score = 0;

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

	GLuint VAOup = createTriangle(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
	GLuint VAOdown = createTriangle(0.0, 0.0, 1.0, 1.0, 1.0, 0.0);

	generateQuads();

	glUseProgram(shaderID);

	// Enviando a cor desejada (vec4) para o fragment shader
	// Utilizamos a variáveis do tipo uniform em GLSL para armazenar esse tipo de info
	// que não está nos buffers
	GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

	//Matriz de projeção paralela ortográfica
	glm::mat4 projection = glm::ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

	glm::mat4 model;


	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);


		for (int i = 0; i < quads.size(); i++)
		{
			glBindVertexArray(VAOup);
			model = glm::mat4(1);
			model = glm::translate(model, quads[i].bottom_left_position);
			model = glm::scale(model, quads[i].dimensions);
			glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));
			glUniform4f(colorLoc, quads[i].color.r, quads[i].color.g, quads[i].color.b, quads[i].color.a);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);


			glBindVertexArray(VAOdown);
			glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));
			glUniform4f(colorLoc, quads[i].color.r, quads[i].color.g, quads[i].color.b, quads[i].color.a);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);
		}

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAOup);
	glDeleteVertexArrays(1, &VAOdown);
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

GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2) {
	GLfloat vertices[] = {
		x0, y0, 0.0f,
		x1, y1, 0.0f,
		x2, y2, 0.0f
	};

	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return VAO;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (quads.empty())
		{
			generateQuads();
			score = 0;
		}
		else
		{
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			ypos = HEIGHT - ypos;
			for (int i = 0; i < quads.size(); i++)
			{
				if (xpos > quads[i].bottom_left_position.x && xpos < quads[i].bottom_left_position.x + quads[i].width	//identifica em qual retângulo 
					&& ypos > quads[i].bottom_left_position.y && ypos < quads[i].bottom_left_position.y + quads[i].height) //ocorreu o clique do mouse
				{
					glm::vec4 temporary_color = quads[i].color;
					std::vector<std::vector<Quad>::iterator> items_to_remove;
					float color_threshold = 0.2;
					for (auto it = quads.begin(); it != quads.end(); it++)
					{
						if (glm::distance(temporary_color, (*it).color) <= color_threshold)
						{
							items_to_remove.push_back(it);
						}
					}
					score = score + glm::pow(items_to_remove.size() * 5, 2);
					std::cout << "Score: " << score << std::endl;
					for (auto it = items_to_remove.rbegin(); it != items_to_remove.rend(); it++)
					{
						quads.erase(*it);
					}
					if (quads.empty())
					{
						std::cout << "clique na tela novamente para reiniciar" << std::endl;
					}
					return;
				}
			}
		}
	}
}

void generateQuads()
{
	std::uniform_real_distribution<> dist(0.0, 1.0);
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			Quad quadrado;
			quadrado.bottom_left_position = glm::vec3(i * quadrado.width, j * quadrado.height, 0.0);
			float r = dist(gen);
			float g = dist(gen);
			float b = dist(gen);
			quadrado.color = glm::vec4(r, g, b, 1.0);
			quads.push_back(quadrado);
		}
	}
}
