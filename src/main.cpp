#include "Window.h"
#include "Shader.h"
#include "raymath.h"
#include <cstddef>

struct Vertex
{
    Vector2 pos;   // offset of 0
    Vector3 col;   // offset of 8 (4 bytes for pos.x + 4 bytes for pos.y = 8)
};

static const Vertex vertices_white[3] =
{
    { { -0.6f, -0.4f }, { 1.0f, 0.0f, 0.0f } },
    { {  0.6f, -0.4f }, { 1.0f, 0.0f, 0.0f } },
    { {   0.f,  0.6f }, { 1.0f, 0.0f, 0.0f } }
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

int main()
{
    CreateWindow(800, 800, "Graphics 1");

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
        glClear(GL_COLOR_BUFFER_BIT);

        Loop();
    }

    return 0;
}