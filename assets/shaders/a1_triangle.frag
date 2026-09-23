#version 330 core
out vec4 fragColor;
in vec3 color;

uniform vec3 u_color;

void main()
{
    fragColor = vec4(color * u_color, 1.0);
	//fragColor = vec4(0.1f,0.1f,0.9f, 1.0f);
}
