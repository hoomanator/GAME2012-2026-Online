#include "Window.h"
#include "Shader.h"
#include "Buffer.h"
#include "Mesh.h"

#include "raymath.h"
#include <imgui/imgui.h>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <random>

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 800;



static const int line_vertex_count = 8;
static const Vector2 line_vertex_positions[line_vertex_count]{
	{ -1.0f, -1.0f }, // bottom-left
	{ 1.0f, -1.0f },  // bottom-right

	{ 1.0f, -1.0f }, // bottom-right
	{ 1.0f, 1.0f },  // top-right

	{ 1.0f, 1.0f },  // top-right
	{ -1.0f, 1.0f }, // top-left

	{ -1.0f, 1.0f }, // top-left
	{ -1.0f, -1.0f } // bottom-left
};

struct Vertex
{
	Vector2 pos;   // offset of 0
	Vector3 col;   // offset of 8 (4 bytes for pos.x + 4 bytes for pos.y = 8)
};

// Assignment 1 object 1 -- white triangle (change these vertex colours from red to white)!!!
static const Vertex vertices_white[3] =
{
	{ { -0.6f, -0.4f },{ 1.0f, 1.0f, 1.0f } },
	{ { 0.6f, -0.4f },{ 1.0f, 1.0f, 1.0f } },
	{ { 0.f,  0.6f },{ 1.0f, 1.0f, 1.0f } }
};

// Assignment 1 object 2 -- rainbow triangle (done for you)
//static const Vertex vertices_rainbow[3] =
//{
//    { { -0.6f, -0.4f }, { 1.0f, 0.0f, 0.0f } },
//    { {  0.6f, -0.4f }, { 0.0f, 1.0f, 0.0f } },
//    { {   0.f,  0.6f }, { 0.0f, 0.0f, 1.0f } }
//};

static const Vector2 vertex_positions[3] =
{
	{ -0.6f, -0.4f },
	{ 0.6f, -0.4f },
	{ 0.f,  0.6f }
};

static const Vector3 vertex_colors[3] =
{
	{ 1.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f }
};


static const int square_count = 8;
static const int square_vertex_count = square_count * line_vertex_count;

enum ProjectionType : int
{
	PERSPECTIVE = 0,
	ORTHOGRAPHIC = 1
};

struct Line
{
	Vector2 start;
	Vector2 end;
};


int main()
{

	// Calculates new vertices of nested squares based on the midpoints
	Vector2 square_vertex_positions[square_vertex_count];
	for (int i = 0; i < line_vertex_count; i++)
		square_vertex_positions[i] = line_vertex_positions[i];

	for (int s = 1; s < square_count; s++)
	{
		const Vector2* prev = square_vertex_positions + (s - 1) * line_vertex_count;
		Vector2* curr = square_vertex_positions + s * line_vertex_count;
		for (int e = 0; e < 4; e++)
		{
			int next = (e + 1) % 4;
			curr[e * 2 + 0] = Vector2Lerp(prev[e * 2], prev[e * 2 + 1], 0.5f);
			curr[e * 2 + 1] = Vector2Lerp(prev[next * 2], prev[next * 2 + 1], 0.5f);
		}
	}


	CreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Week 9");


	//shaders

	GLuint a1_tri_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/a1_triangle.vert");
	GLuint a1_tri_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/a1_triangle.frag");
	GLuint a1_tri_shader = CreateProgram(a1_tri_vert, a1_tri_frag);

	///////////////////////////////LINE SHADER //////////////////////////////

	GLuint a2_lines_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/a2_lines.vert");
	GLuint a2_lines_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/a2_lines.frag");
	GLuint a2_lines_shader = CreateProgram(a2_lines_vert, a2_lines_frag);

	// Vertex shaders:
	GLuint vs = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/default.vert");
	GLuint vsLines = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/lines.vert");
	GLuint vsVertexPositionColor = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/vertex_color.vert");
	GLuint vsColorBufferColor = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/buffer_color.vert");

	// Fragment shaders:
	GLuint fsLines = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/lines.frag");
	GLuint fsUniformColor = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/uniform_color.frag");
	GLuint fsVertexColor = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/vertex_color.frag");
	GLuint fsTcoords = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/tcoord_color.frag");
	GLuint fsNormals = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/normal_color.frag");

	// Shader programs:
	GLuint shaderUniformColor = CreateProgram(vs, fsUniformColor);
	GLuint shaderVertexPositionColor = CreateProgram(vsVertexPositionColor, fsVertexColor);
	GLuint shaderVertexBufferColor = CreateProgram(vsColorBufferColor, fsVertexColor);
	GLuint shaderLines = CreateProgram(vsLines, fsLines);
	GLuint shaderTcoords = CreateProgram(vs, fsTcoords);
	GLuint shaderNormals = CreateProgram(vs, fsNormals);

	// Add the triangle shaders
	GLuint triangleVs = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/vertex_color.vert");
	GLuint triangleFs = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/vertex_color.frag");
	GLuint shaderTriangle = CreateProgram(triangleVs, triangleFs);

	// Add point shader
	GLuint vsPoints = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/points.vert");
	GLuint shaderPoints = CreateProgram(vsPoints, fsVertexColor);




	GLuint vertex_buffer_rainbow_positions;
	GLuint vertex_buffer_rainbow_colors;
	glGenBuffers(1, &vertex_buffer_rainbow_positions);
	glGenBuffers(1, &vertex_buffer_rainbow_colors);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_rainbow_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_rainbow_colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_colors), vertex_colors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	// Can only have 1 vertex buffer bound at a time, so must unbind in order to prevent overwriting it

	GLuint vertex_buffer_white;
	glGenBuffers(1, &vertex_buffer_white);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_white);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_white), vertices_white, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	GLuint vertex_array_rainbow;
	glGenVertexArrays(1, &vertex_array_rainbow);
	glBindVertexArray(vertex_array_rainbow);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_rainbow_positions);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_rainbow_colors);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);

	glBindVertexArray(GL_NONE);

	GLuint vertex_array_white;
	glGenVertexArrays(1, &vertex_array_white);
	glBindVertexArray(vertex_array_white);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_white);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col));

	glBindVertexArray(GL_NONE);




	GLuint vbo_line_positions = GenVertexBuffer();
	BindVertexBuffer(vbo_line_positions);
	UpdateVertexBuffer(vbo_line_positions, (void*)square_vertex_positions, sizeof(square_vertex_positions));
	UnbindVertexBuffer();

	GLuint vao_line = GenVertexArray();
	BindVertexArray(vao_line);

	EnableVertexAttribute(0);
	BindVertexBuffer(vbo_line_positions);
	SetVertexAttribute(0, 2, GL_FLOAT, sizeof(Vector2));

	UnbindVertexArray();
	UnbindVertexBuffer();


	//////////////////////// Render 1 line for every horizontal pixel ///////////////

	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> start_y(-1.0f, 1.0f);
	std::uniform_real_distribution<float> end_y(-1.0f, 1.0f);
	std::uniform_real_distribution<float> col_r(0.0f, 1.0f);

	std::vector<Line> lines(SCREEN_WIDTH);
	for (int i = 0; i < SCREEN_WIDTH; i++)
	{
		Line& line = lines[i];
		float x = Remap(i, 0, SCREEN_WIDTH, -1.0f, 1.0f);
		line.start.x = lines[i].end.x = x;

		lines[i].start.y = start_y(rng);
		lines[i].end.y = end_y(rng);
		printf("Line %d: start=(%f, %f), end=(%f, %f)\n", i, lines[i].start.x, lines[i].start.y, lines[i].end.x, lines[i].end.y);
	}

	std::vector<Vector3> linesColor(SCREEN_WIDTH * 2);
	for (int i = 0; i < SCREEN_WIDTH * 2; i++)
	{
		linesColor[i] = Vector3{ col_r(rng), col_r(rng), col_r(rng) };
	}




	/*GLuint vbo_lines;
	glGenBuffers(1, &vbo_lines);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_lines);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lines) * SCREEN_WIDTH, lines.data(), GL_STATIC_DRAW);


	GLuint cbo_lines;
	glGenBuffers(1, &cbo_lines);
	glBindBuffer(GL_ARRAY_BUFFER, cbo_lines);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linesColor) * SCREEN_WIDTH, linesColor.data(), GL_STATIC_DRAW);


	GLuint vao_lines;
	glGenVertexArrays(1, &vao_lines);
	glBindVertexArray(vao_lines);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_lines);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, cbo_lines);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);*/

	////////////////////////////////////////////////////////////////////////////

	// Create Mesh
	//An OBJ file stores the physical 3D geometry of a model (such as vertices, faces, normals, and UV coordinates), while an MTL (Material Template Library) file stores the appearance and surface properties (such as colors, shininess, transparency, and links to texture images)
	//git add -f *.obj  (your gitignore file may be ignoring .obj files, so you may need to force add them to your repo)
	Mesh shapeMesh, cubeMesh, sphereMesh, knotMesh, planeMesh, headMesh, rockMesh;
	CreateMesh(&shapeMesh, PLANE);
	CreateMesh(&cubeMesh, CUBE);
	CreateMesh(&sphereMesh, SPHERE);
	CreateMesh(&knotMesh, KNOT);
	CreateMesh(&rockMesh, ROCK);
	CreateMesh(&planeMesh, "assets/meshes/plane.obj");
	CreateMesh(&headMesh, "assets/meshes/head.obj");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_LINE_SMOOTH);


	//////////////////////////////////////////////////////////////////////


	int object_index = 5;
	int projection_type = ORTHOGRAPHIC;

	float aspect = (float)WindowWidth() / (float)WindowHeight();
	float near = 0.1f;
	float far = 100.0f;
	float fov = 75.0f * DEG2RAD;

	float left = -1.1;
	float right = 1.1;
	float bottom = -1.1;
	float top = 1.1;

	bool obj_translate = false;
	bool obj_rotate = false;
	bool obj_scale = false;

	/* Loop until the user closes the window */
	while (!WindowShouldClose())
	{
		if (IsKeyPressed(KEY_ESCAPE))
			SetWindowShouldClose(true);

		if (IsKeyPressed(KEY_SPACE))
		{
			++object_index %= 10;
		}


		// Colors are represented as fractions between 0.0 and 1.0, so convert using a colour-picker tool accordingly!
		float r = 239.0f / 255.0f;
		float g = 136.0f / 255.0f;
		float b = 190.0f / 255.0f;
		float a = 1.0f;

		// Time in seconds since GLFW was initialized (use this with functions like sinf and cosf for repeating animations)

		float tt = Time();

		Matrix world = MatrixIdentity();

		if (obj_translate)
		{
			world = MatrixTranslate(cosf(tt) * 0.4f + 0.5f, sin(tt) * 0.4f + 0.5f, 1.f);
		}

		if (obj_rotate)
		{
			world = MatrixRotateZ(tt * 100 * DEG2RAD);
		}

		if (obj_scale)
		{
			world = MatrixScale(cosf(tt) * 0.4f + 0.5f, cosf(tt) * 0.4f + 0.5f, 1.f);
		}

		Matrix view = MatrixLookAt({ 0.f, 0.f, 10.f }, { 0.f, 0.f, 0.f }, Vector3UnitY);

		Matrix proj = MatrixIdentity();
		switch (projection_type)
		{
		case PERSPECTIVE:
			proj = MatrixPerspective(fov, aspect, near, far);
			break;
		case ORTHOGRAPHIC:
			proj = MatrixOrtho(left, right, bottom, top, near, far);
			break;
		}

		Matrix mvp = world * view * proj;

		/* Render here */
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		switch (object_index)
		{
		case 0:
			glLineWidth(5.0f);
			BindVertexArray(vao_line);
			BeginShader(a2_lines_shader);
			{
				SendMat4(mvp, "u_mvp");
				SendVec3(Vector3UnitX, "u_color");
				glDrawArrays(GL_LINES, 0, square_vertex_count);
			}
			EndShader();
			UnbindVertexArray();
			//glLineWidth(1.0f);
			break;

		case 1:
		{
			/*glUseProgram(shaderLines);
			GLint u_color = glGetUniformLocation(shaderLines, "u_color");
			GLint u_mvp = glGetUniformLocation(shaderLines, "u_mvp");
			glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp));
			glUniform3f(u_color, 1.0f, 1.0f, 1.0f);
			glBindVertexArray(vao_lines);
			glDrawArrays(GL_LINES, 0, SCREEN_WIDTH * 2);*/
			break;
		}
		case 2:
		{
			mvp = world * view * proj;


			BindVertexArray(vertex_array_white);
			BeginShader(a1_tri_shader);
			{
				SendMat4(mvp, "u_mvp");
				SendVec3(Vector3UnitX, "u_color");
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}
			EndShader();
			UnbindVertexArray();

			//glUseProgram(a1_tri_shader);

			//GLint u_mvp = glGetUniformLocation(a1_tri_shader, "u_mvp");
			//glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp));

			//GLint u_color = glGetUniformLocation(a1_tri_shader, "u_color");
			//glUniform3f(u_color, 0.0f, 1.0f, 0.0f);

			//glBindVertexArray(vertex_array_white);
			//glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		}
		case 3:
		{
			Matrix s = MatrixIdentity();
			Matrix r = MatrixIdentity();
			Matrix t = MatrixIdentity();
			// Matrix view = MatrixLookAt({ 0.0f, 0.0f, 10.0f }, { 0.0f, 0.0f, 0.0f }, Vector3UnitY);
			// Matrix proj = MatrixOrtho(-1.0f, 1.0f, -1.0f, 1.0f, 0.01f, 100.0f);

			float time = Time();
			float a = cosf(time) * 0.5 + 0.5f;
			Vector3 A = { -1.0f, -1.0f, 0.0f };
			Vector3 B = { 1.0f, 1.0f, 0.0f };
			Vector3 C = Vector3Lerp(A, B, a);
			t = MatrixTranslate(C.x, C.y, C.z);

			GLint u_color = glGetUniformLocation(a1_tri_shader, "u_color");
			GLint u_mvp = glGetUniformLocation(a1_tri_shader, "u_mvp");

			world = s * r * t;
			mvp = world * view * proj;
			glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp));

			glUseProgram(a1_tri_shader);
			glUniform3f(u_color, 0.8, 0.8f, 0.8f);
			glBindVertexArray(vertex_array_rainbow);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		}

		case 4:
		{
			GLint u_color = glGetUniformLocation(a1_tri_shader, "u_color");
			GLint u_mvp = glGetUniformLocation(a1_tri_shader, "u_mvp");

			float time = Time();
			float a = cosf(time) * 0.5 + 0.5f;

			//Translation interpolation
			Vector3 tA = { 0.0f, -10.0f, 0.0f };
			Vector3 tB = { 0.0f, 10.0f, 0.0f };
			Vector3 tC = Vector3Lerp(tA, tB, a);

			//Scale interpolation
			Vector3 sA = { 1.0f, 1.0f, 1.0f };
			Vector3 sB = { 10.0f, 10.0f, 1.0f };
			Vector3 sC = Vector3Lerp(sA, sB, a);

			//Spherical Lerp because we interpolate between two quaternions (rotations) instead of two vectors (positions)
			Quaternion qA = QuaternionIdentity();
			Quaternion qB = QuaternionFromEuler(0.0f, 0.0f, 90.0f * DEG2RAD);
			Quaternion qC = QuaternionSlerp(qA, qB, a);

			Matrix s = MatrixScale(sC.x, sC.y, sC.z);
			Matrix r = QuaternionToMatrix(qC);
			Matrix t = MatrixTranslate(tC.x, tC.y, tC.z);

			//Color interpolation
			Vector3 cA = Vector3UnitY;  //Green
			Vector3 cB = Vector3UnitZ; //Blue 
			Vector3 cC = Vector3Lerp(cA, cB, a);



			Matrix mvp = s * r * t * view * proj;

			glUseProgram(a1_tri_shader);
			glUniform3f(u_color, 0.5, 0.5f, 0.5f);

			glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp));
			glUniform3f(u_color, cC.x, cC.y, cC.z);

			glBindVertexArray(vertex_array_rainbow);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		}
		case 5:
		{
			glUseProgram(shaderTcoords);

			GLint u_mvp = glGetUniformLocation(shaderTcoords, "u_mvp");
			Matrix world2 = MatrixRotateY(100.0f * tt * DEG2RAD);
			Matrix mvp2 = world2 * view * proj;
			glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp2));

			DrawMesh(shapeMesh);
			break;

		}

		case 6:
		{
			glUseProgram(shaderNormals);

			GLint u_mvp = glGetUniformLocation(shaderNormals, "u_mvp");
			Matrix world2 = MatrixRotateY(100.0f * tt * DEG2RAD) * MatrixRotateX(100.0f * tt * DEG2RAD);
			Matrix mvp2 = world2 * view * proj;
			glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp2));

			DrawMesh(cubeMesh);
			break;
		}

		case 7:
		{
			glUseProgram(shaderNormals);

			GLint u_mvp = glGetUniformLocation(shaderNormals, "u_mvp");
			Matrix world2 = MatrixRotateY(100.0f * tt * DEG2RAD);
			Matrix mvp2 = world2 * view * proj;
			glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp2));

			DrawMesh(headMesh);
			break;
		}

		case 8:
		{
			glUseProgram(shaderNormals);

			GLint u_mvp = glGetUniformLocation(shaderTcoords, "u_mvp");
			glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp));

			DrawMesh(knotMesh);
			break;
		}

		case 9:
		{
			glUseProgram(shaderNormals);

			GLint u_mvp = glGetUniformLocation(shaderTcoords, "u_mvp");
			glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp));

			DrawMesh(rockMesh);
			break;
		}

		}

		// Write our widget code within begin/end gui loop!
		BeginGui();
		ImGui::RadioButton("Orthographic", &projection_type, PERSPECTIVE); ImGui::SameLine();
		ImGui::RadioButton("Perspective", &projection_type, ORTHOGRAPHIC);

		ImGui::Checkbox("Translate", &obj_translate);
		ImGui::Checkbox("Rotate", &obj_rotate);
		ImGui::Checkbox("Scale", &obj_scale);

		ImGui::SliderFloat("Left", &left, -10.0f, 0.0f);
		ImGui::SliderFloat("Right", &right, 0.0f, 10.0f);
		ImGui::SliderFloat("Bottom", &bottom, -10.0f, 0.0f);
		ImGui::SliderFloat("Top", &top, 0.0f, 10.0f);

		ImGui::SliderAngle("FoV", &fov, 5.0f, 175.0f);
		ImGui::SliderFloat("Near", &near, 0.001f, 1.0f);
		ImGui::SliderFloat("Far", &far, 1.0f, 1000.0f);

		// Extra practice: add additional widgets such sliders for camera position & camera direction!
		//ImGui::ShowDemoWindow(nullptr);
		EndGui();

		// Called at end of the frame to swap buffers and update input
		Loop();
	}

	glDeleteVertexArrays(1, &vertex_array_rainbow);
	glDeleteVertexArrays(1, &vertex_array_white);
	glDeleteBuffers(1, &vertex_buffer_rainbow_positions);
	glDeleteBuffers(1, &vertex_buffer_rainbow_colors);
	glDeleteBuffers(1, &vertex_buffer_white);
	glDeleteProgram(a1_tri_shader);
	glDeleteShader(a1_tri_frag);
	glDeleteShader(a1_tri_vert);

	DestroyWindow();
	return 0;
}