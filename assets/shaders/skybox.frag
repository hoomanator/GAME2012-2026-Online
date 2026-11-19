#version 330 core

in vec3 position;

uniform samplerCube u_cubemap;

out vec4 FragColor;

void main()
{
    vec3 color = texture(u_cubemap, position).rgb;
    FragColor = vec4(color, 1.0);
}
