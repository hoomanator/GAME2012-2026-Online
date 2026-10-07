#include "Window.h"
#include "Shader.h"
#include "imgui/imgui.h"
#include "raymath.h"
#include <cstddef>

struct Vertex
{
    Vector2 pos;   // offset of 0
    Vector3 col;   // offset of 8 (4 bytes for pos.x + 4 bytes for pos.y = 8)
};

static const Vertex vertices_white[3] =
{
    { { -0.6f, -0.4f }, { 1.0f, 1.0f, 1.0f } },
    { {  0.6f, -0.4f }, { 1.0f, 1.0f, 1.0f } },
    { {   0.f,  0.6f }, { 1.0f, 1.0f, 1.0f } }
};

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

enum ProjectionType : int
{
	PERSPECTIVE = 0,
	ORTHOGRAPHIC = 1    
};

int main()
{
    CreateWindow(800, 800, "Week 5");

    GLuint a1_tri_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/a1_triangle.vert");
    GLuint a1_tri_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/a1_triangle.frag");
    GLuint a1_tri_shader = CreateProgram(a1_tri_vert, a1_tri_frag);


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

    int object_index = 0;


	int projection_type = PERSPECTIVE;


    GLint u_color = glGetUniformLocation(a1_tri_shader, "u_color");
	GLint u_mvp = glGetUniformLocation(a1_tri_shader, "u_mvp");

	float aspect = (float)WindowWidth() / (float)WindowHeight();
	float near = 0.1f;
	float far = 100.0f;
	float fov = 75.0f * DEG2RAD;

    float left = -1.0;
	float right = 1.0;
	float bottom = -1.0;
	float top = 1.0;

    bool obj_translate = false;
	bool obj_rotate = false;
	bool obj_scale = false;
    


    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ESCAPE))
            SetWindowShouldClose(true);

        // Colors are represented as fractions between 0.0 and 1.0, so convert using a colour-picker tool accordingly!
        float r = 239.0f / 255.0f;
        float g = 136.0f / 255.0f;
        float b = 190.0f / 255.0f;
        float a = 1.0f;

        /* Render here */
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (IsKeyPressed(KEY_SPACE))
        {
            ++object_index %= 5;
        }

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

        switch (object_index)
        {
        case 0:
            glUseProgram(a1_tri_shader);

			//world = MatrixScale(2.f, 2.f, 2.f) * MatrixTranslate(5.f, 5.f, 0.f);
            mvp = world * view * proj;
			glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp));
            glUniform3f(u_color, 0.0f, 1.0f, 0.0f);
            glBindVertexArray(vertex_array_white);
            glDrawArrays(GL_TRIANGLES, 0, 3);           

            break;
        case 1:
			glPointSize(10);
            glUseProgram(a1_tri_shader);
            world = MatrixIdentity();
			mvp = world * view * proj;
            glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp));
            glUniform3f(u_color, 1.0f, 1.0f, 1.0f);
            glBindVertexArray(vertex_array_white);
            glDrawArrays(GL_POINTS, 0, 3);
            break;
        case 2:
            glUseProgram(a1_tri_shader);
            world = MatrixScale(2.f, 2.f, 2.f);
            mvp = world * view * proj;
            glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp));
            glUniform3f(u_color, 1.0f, 1.0f, 1.0f);
            glBindVertexArray(vertex_array_rainbow);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;
        case 3:
        {
            glUseProgram(a1_tri_shader);

			float time = Time();
			float a = cosf(time) * 0.5f + 0.5f;
            Vector3 A = { -10.0f, 10.0f, 0.0f };
            Vector3 B = { 10.0f, -10.0f, 0.0f };
			Vector3 C = Vector3Lerp(A, B, a);

			Matrix s = MatrixScale(2.f, 2.f, 2.f);
			Matrix r = MatrixRotateZ(0.0f * DEG2RAD);
            Matrix t = MatrixTranslate(C.x, C.y, C.z);

			Matrix world = s * r * t;
			Matrix view = MatrixLookAt({ 0.f, 0.f, 10.f }, { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f });
			Matrix proj = MatrixOrtho(-10.f, 10.f, -10.f, 10.f, 0.1f, 100.f);
			Matrix mvp = world * view * proj;

            glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp));

            glUniform3f(u_color, 1.0f, 1.0f, 1.0f);
            glBindVertexArray(vertex_array_rainbow);
            glDrawArrays(GL_TRIANGLES, 0, 3);


            Vector3 A1 = { -10.0f, -10.0f, 0.0f };
            Vector3 B1 = { 10.0f, 10.0f, 0.0f };
            Vector3 C1 = Vector3Lerp(A1, B1, a);

            Matrix s1 = MatrixScale(2.f, 2.f, 2.f);
            Matrix r1 = MatrixRotateZ(0.0f * DEG2RAD);
            Matrix t1 = MatrixTranslate(C1.x, C1.y, C1.z);

            Matrix world1 = s1 * r1 * t1;
            Matrix view1 = MatrixLookAt({ 0.f, 0.f, 10.f }, { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f });
            Matrix proj1 = MatrixOrtho(-10.f, 10.f, -10.f, 10.f, 0.1f, 100.f);
            Matrix mvp1 = world1 * view1 * proj1;

            glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp1));

            glUniform3f(u_color, 1.0f, 1.0f, 1.0f);
            glBindVertexArray(vertex_array_rainbow);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            break;
        }
        case 4:
        {

            glUseProgram(a1_tri_shader);

            Matrix view = MatrixLookAt({ 0.f, 0.f, 10.f }, { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f });
            Matrix proj = MatrixOrtho(-10.f, 10.f, -10.f, 10.f, 0.1f, 100.f);

            float time = Time();
            float a = cosf(time) * 0.5f + 0.5f;

            //Translation Interpolation
            Vector3 tA = { -10.0f, 10.0f, 0.0f };
            Vector3 tB = { 10.0f, -10.0f, 0.0f };
            Vector3 tC = Vector3Lerp(tA, tB, a);

			Vector3 sA = { 1.0f, 1.0f, 1.0f };
            Vector3 sB = { 5.0f, 5.0f, 1.0f };
            Vector3 sC = Vector3Lerp(sA, sB, a);

			Quaternion qA = QuaternionIdentity();
			Quaternion qB = QuaternionFromEuler( 0.0f, 0.0f, 180.0f * DEG2RAD);   
			Quaternion qC = QuaternionSlerp(qA, qB, a);

			Matrix s = MatrixScale(sC.x, sC.y, sC.z);
			Matrix r = QuaternionToMatrix(qC);
            Matrix t = MatrixTranslate(tC.x, tC.y, tC.z);

            Matrix world = s * r * t;
            Matrix mvp = world * view * proj;

            glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MatrixToFloat(mvp));

         
            glUniform3f(u_color, 1.0f, 1.0f, 1.0f);
            glBindVertexArray(vertex_array_rainbow);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;
        }
        default:
            break;
        }

     
		BeginGui();
        //ImGui::Begin("Hello, world!");
        ImGui::Text("This is some useful text.");
		ImGui::RadioButton("Perspective", &projection_type, PERSPECTIVE); ImGui::SameLine();
		ImGui::RadioButton("Orthographic", &projection_type, ORTHOGRAPHIC);

		ImGui::Checkbox("Translate", &obj_translate); 
		ImGui::Checkbox("Rotate", &obj_rotate); 
		ImGui::Checkbox("Scale", &obj_scale);

        ImGui::SliderFloat("Left", &left, -10.0f, 10.0f);
        ImGui::SliderFloat("Right", &right, -10.0f, 10.0f);     
        ImGui::SliderFloat("Bottom", &bottom, -10.0f, 10.0f);
        ImGui::SliderFloat("Top", &top, -10.0f, 10.0f);

		ImGui::SliderFloat("Near", &near, 0.1f, 10.0f);
		ImGui::SliderFloat("Far", &far, 10.0f, 100.0f);
		ImGui::SliderAngle("FOV", &fov, 1.0f, 179.0f);  

		//ImGui::ShowDemoWindow(nullptr);
		EndGui();

        Loop();
    }

    return 0;
}