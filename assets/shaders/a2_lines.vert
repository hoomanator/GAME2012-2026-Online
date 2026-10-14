#version 330 core
layout (location = 0) in vec2 vPos;

uniform mat4 u_mvp;

void main()
{
    vec4 pos = vec4(vPos, 0.0, 1.0);
    gl_Position = u_mvp * pos;
}
