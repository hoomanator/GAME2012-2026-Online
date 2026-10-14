#version 330 core
out vec4 fragColor;
in vec3 color;

uniform vec3 u_color;

void main()
{
    fragColor = vec4(u_color, 1.0);
}
