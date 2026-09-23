#version 330 core
in vec3 vCol;
in vec2 vPos;

out vec3 color;

void main()
{
    gl_Position = vec4(vPos, 0.0, 1.0);
    color = vCol;
}
