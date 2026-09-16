#include "Window.h"
#include <glad/glad.h>
#include <cstddef>

struct vec2 {
	float x;
	float y;
};

struct vec3
{
	float r;
	float g;
	float b;
};

struct Vertex {
	vec2 pos;
	vec3 col;
};

static const Vertex vertices[] = {
	{ { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
	{ {  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
	{ {  0.0f,  0.5f }, { 1.0f, 0.0f, 0.0f } },

	{ { -0.0f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
	{ {  1.0f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
	{ {  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f } },
};

static const char* vertex_shader_text = R"(
#version 330 core
in vec3 vCol;
in vec2 vPos;

out vec3 color;

void main()
{
    gl_Position = vec4(vPos, 0.0, 1.0);
    color = vCol;
}
)";

static const char* fragment_shader_text = R"(		
#version 330 core
out vec4 FragColor;
in vec3 color;

void main()
{
    FragColor = vec4(color, 1.0);
	//FragColor = vec4(0.1f,0.1f,0.9f, 1.0f);
}
)";

int main() {
	CreateWindow(800, 600, "OpenGL Triangle");

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);

	const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);

	const GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	const GLuint vpos_location = glGetAttribLocation(shader_program, "vPos");
	const GLuint vcol_location = glGetAttribLocation(shader_program, "vCol");

	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));


	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col));

	glPointSize(15.0f);

	while (!WindowShouldClose()) {
		
		if (IsKeyPressed(KEY_ESCAPE)) {
			SetWindowShouldClose(true);
		}

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glUseProgram(shader_program);
		glBindVertexArray(vertex_array);
		//glDrawArrays(GL_LINE_LOOP, 0, 3);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawArrays(GL_POINTS, 0, 6);
		//glDrawArrays(GL_LINES, 0, 6);

		Loop();
	}

	DestroyWindow();
	return 0;
}